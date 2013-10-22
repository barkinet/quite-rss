/* ============================================================
* QuiteRSS is a open-source cross-platform RSS/Atom news feeds reader
* Copyright (C) 2011-2013 QuiteRSS Team <quiterssteam@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "faviconobject.h"
#include "VersionNo.h"

#include <QDebug>
#include <QtSql>
#ifdef HAVE_QT5
#include <QWebPage>
#else
#include <qwebkitversion.h>
#endif

#define REPLY_MAX_COUNT 4
#define REQUEST_TIMEOUT 30

FaviconObject::FaviconObject(QObject *parent)
  : QObject(parent)
{
  setObjectName("faviconObject_");

  QTimer *timeout_ = new QTimer();
  connect(timeout_, SIGNAL(timeout()), this, SLOT(slotRequestTimeout()));
  timeout_->start(1000);

  getUrlTimer_ = new QTimer();
  getUrlTimer_->setSingleShot(true);
  getUrlTimer_->setInterval(20);
  connect(getUrlTimer_, SIGNAL(timeout()), this, SLOT(getQueuedUrl()));

  connect(this, SIGNAL(signalGet(QUrl,QString,int)),
          SLOT(slotGet(QUrl,QString,int)));

  networkManager_ = new NetworkManager(this);
  connect(networkManager_, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(finished(QNetworkReply*)));
}

/** @brief Put requested URL in request queue
 *----------------------------------------------------------------------------*/
void FaviconObject::requestUrl(QString urlString, QString feedUrl)
{
  urlsQueue_.enqueue(urlString);
  feedsQueue_.enqueue(feedUrl);
  getUrlTimer_->start();
}

/** @brief Process request queue by timer
 *----------------------------------------------------------------------------*/
void FaviconObject::getQueuedUrl()
{
  if (currentFeeds_.size() >= REPLY_MAX_COUNT) {
    getUrlTimer_->start();
    return;
  }

  if (!urlsQueue_.isEmpty()) {
    getUrlTimer_->start();

    QString feedUrl = feedsQueue_.head();

    if (hostList_.contains(QUrl(feedUrl).host())) {
      int count = 0;
      foreach (QString url, currentFeeds_) {
        if (QUrl(url).host() == QUrl(feedUrl).host()) {
          if (++count >= 1) {
            return;
          }
        }
      }
    }

    QString urlString = urlsQueue_.dequeue();
    feedUrl = feedsQueue_.dequeue();

    QUrl url = QUrl::fromEncoded(urlString.toUtf8());
    if (!url.isValid()) {
      url = QUrl::fromEncoded(feedUrl.toUtf8());
    }
    emit signalGet(url, feedUrl, 0);
  }
}

/** @brief Prepare and send network request to receive all data
 *----------------------------------------------------------------------------*/
void FaviconObject::slotGet(const QUrl &getUrl, const QString &feedUrl, const int &cnt)
{
  QNetworkRequest request(getUrl);
  QString userAgent = QString("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/%1 (KHTML, like Gecko) QuiteRSS/%2 Safari/%1").
      arg(qWebKitVersion()).arg(STRPRODUCTVER);
  request.setRawHeader("User-Agent", userAgent.toUtf8());
  request.setRawHeader("Accept-Language", "en-us,en");

  currentUrls_.append(getUrl);
  currentFeeds_.append(feedUrl);
  currentCntRequests_.append(cnt);
  currentTime_.append(REQUEST_TIMEOUT);

  QNetworkReply *reply = networkManager_->get(request);
  requestUrl_.append(reply->url());
  networkReply_.append(reply);
}

/** @brief Finish network request processing
 *----------------------------------------------------------------------------*/
void FaviconObject::finished(QNetworkReply *reply)
{
  int currentReplyIndex = currentUrls_.indexOf(reply->url());
  if (currentReplyIndex >= 0) {
    currentTime_.removeAt(currentReplyIndex);
    QUrl url = currentUrls_.takeAt(currentReplyIndex);
    QString feedUrl = currentFeeds_.takeAt(currentReplyIndex);
    int cntRequests = currentCntRequests_.takeAt(currentReplyIndex);

    if(reply->error() == QNetworkReply::NoError) {
      QUrl redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
      if (redirectionTarget.isValid()) {
        if ((cntRequests == 0) || (cntRequests == 1)) {
          if (redirectionTarget.host().isNull())
            redirectionTarget.setUrl("http://"+url.host()+redirectionTarget.toString());
          emit signalGet(redirectionTarget, feedUrl, cntRequests+2);
        }
      } else {
        QByteArray data = reply->readAll();
        if (!data.isNull()) {
          if ((cntRequests == 0) || (cntRequests == 2)) {
            QString linkFavicon;
            QString str = QString::fromUtf8(data);
            if (str.contains("<html", Qt::CaseInsensitive)) {
              QRegExp rx("<link[^>]+rel=['\"]icon['\"][^>]+>",
                         Qt::CaseInsensitive, QRegExp::RegExp2);
              int pos = rx.indexIn(str);
              if (pos == -1) {
                rx = QRegExp("<link[^>]+rel=['\"]shortcut icon['\"][^>]+>",
                             Qt::CaseInsensitive, QRegExp::RegExp2);
                pos = rx.indexIn(str);
              }
              if (pos > -1) {
                str = rx.cap(0);
                rx.setPattern("href=\"([^\"]+)");
                pos = rx.indexIn(str);
                if (pos == -1) {
                  rx.setPattern("href='([^']+)");
                  pos = rx.indexIn(str);
                }
                if (pos > -1) {
                  linkFavicon = rx.cap(1).simplified();
                  QUrl urlFavicon(linkFavicon);
                  if (urlFavicon.host().isEmpty()) {
                    urlFavicon.setHost(url.host());
                  }
                  if (urlFavicon.scheme().isEmpty()) {
                    urlFavicon.setScheme(url.scheme());
                  }
                  linkFavicon = urlFavicon.toString().simplified();
                  qDebug() << "Favicon URL:" << linkFavicon;
                  emit signalGet(linkFavicon, feedUrl, cntRequests+1);
                }
              }
            }
            if (linkFavicon.isEmpty()) {
              if ((cntRequests == 0) || (cntRequests == 2)) {
                QString link = QString("%1://%2/favicon.ico").arg(url.scheme()).arg(url.host());
                emit signalGet(link, feedUrl, cntRequests+1);
              }
            }
          } else {
            // Emit receiced data in main thread
            QFileInfo info(url.path());
            emit signalIconRecived(feedUrl, data, info.suffix());
          }
        } else {
          if ((cntRequests == 0) || (cntRequests == 2)) {
            QString link = QString("%1://%2/favicon.ico").arg(url.scheme()).arg(url.host());
            emit signalGet(link, feedUrl, cntRequests+1);
          }
        }
      }
    } else {
      if (reply->errorString().contains("Service Temporarily Unavailable")) {
        if (!hostList_.contains(QUrl(feedUrl).host())) {
          hostList_.append(QUrl(feedUrl).host());
        }
      }

      if (cntRequests == 0) {
        QString link = QString("%1://%2").arg(url.scheme()).arg(url.host());
        emit signalGet(link, feedUrl, 2);
        qDebug() << "Request Url error: " << reply->url().toString() << reply->errorString();
      }
    }
  } else {
    qCritical() << "Request Url error: " << reply->url().toString() << reply->errorString();
  }

  int replyIndex = requestUrl_.indexOf(reply->url());
  if (replyIndex >= 0) {
    requestUrl_.removeAt(replyIndex);
    networkReply_.removeAt(replyIndex);
  }
  reply->abort();
  reply->deleteLater();
}

/** @brief Timeout to delete requests without answer from server
 *----------------------------------------------------------------------------*/
void FaviconObject::slotRequestTimeout()
{
  for (int i = currentTime_.count() - 1; i >= 0; i--) {
    int time = currentTime_.at(i) - 1;
    if (time <= 0) {
      QUrl url = currentUrls_.takeAt(i);
      QString feedUrl = currentFeeds_.takeAt(i);
      int cntRequests = currentCntRequests_.takeAt(i);
      currentTime_.removeAt(i);

      int replyIndex = requestUrl_.indexOf(url);
      requestUrl_.removeAt(replyIndex);
      QNetworkReply *reply = networkReply_.takeAt(replyIndex);
      reply->deleteLater();

      if (cntRequests == 0) {
        emit signalGet(url, feedUrl, 2);
      }
    } else {
      currentTime_.replace(i, time);
    }
  }
}

/** @brief Save icon in DB and emit signal to update it
 *----------------------------------------------------------------------------*/
void FaviconObject::slotIconSave(QString feedUrl, QByteArray faviconData)
{
  int feedId = 0;

  QSqlQuery q;
  q.prepare("SELECT id FROM feeds WHERE xmlUrl LIKE :xmlUrl");
  q.bindValue(":xmlUrl", feedUrl);
  q.exec();
  if (q.next()) {
    feedId = q.value(0).toInt();
  }

  q.prepare("UPDATE feeds SET image = ? WHERE id == ?");
  q.addBindValue(faviconData.toBase64());
  q.addBindValue(feedId);
  q.exec();

  emit signalIconUpdate(feedId, faviconData);
}
