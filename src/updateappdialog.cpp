#include "updateappdialog.h"
#include "VersionNo.h"

#if defined(Q_OS_WIN)
#include <qt_windows.h>
#endif

UpdateAppDialog::UpdateAppDialog(const QString &lang, QSettings *settings,
                                 QWidget *parent, bool show)
  : QDialog(parent),
    settings_(settings),
    showDialog_(show)
{
  if (showDialog_) {
    setWindowTitle(tr("Check for Updates"));
    setWindowFlags (windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setObjectName("UpdateAppDialog");
    resize(450, 350);

    QVBoxLayout *updateApplayout = new QVBoxLayout(this);
    updateApplayout->setMargin(5);
    updateApplayout->setAlignment(Qt::AlignCenter);

    infoLabel = new QLabel(tr("Checking for updates..."), this);
    infoLabel->setOpenExternalLinks(true);
    updateApplayout->addWidget(infoLabel, 0);

    history_ = new QTextBrowser(this);
    history_->setObjectName("history_");
    history_->setText(tr("Loading history..."));
    history_->setOpenExternalLinks(true);
    updateApplayout->addWidget(history_, 1);

    remindAboutVersion_ = new QCheckBox(tr("Don't remind about this version"), this);
    remindAboutVersion_->setChecked(settings_->value("remindAboutVersion", false).toBool());
    remindAboutVersion_->hide();
    updateApplayout->addWidget(remindAboutVersion_, 0);

    installButton_ = new QPushButton(tr("&Install"), this);
    installButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    installButton_->hide();
    connect(installButton_, SIGNAL(clicked()), SLOT(updaterRun()));

    QPushButton *closeButton = new QPushButton(tr("&Close"), this);
    closeButton->setDefault(true);
    closeButton->setFocus(Qt::OtherFocusReason);
    closeButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(closeButton, SIGNAL(clicked()), SLOT(close()));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(installButton_);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    updateApplayout->addLayout(buttonLayout);

    QString urlHistory;
    if (lang.contains("ru", Qt::CaseInsensitive))
      urlHistory = "http://quite-rss.googlecode.com/hg/HISTORY_RU";
    else urlHistory = "http://quite-rss.googlecode.com/hg/HISTORY_EN";
    historyReply_ = manager_.get(QNetworkRequest(QUrl(urlHistory)));
    connect(historyReply_, SIGNAL(finished()), this, SLOT(slotFinishHistoryReply()));

    connect(this, SIGNAL(finished(int)), this, SLOT(closeDialog()));

    restoreGeometry(settings_->value("updateAppDlg/geometry").toByteArray());
  }

  page.mainFrame()->load(QUrl("http://code.google.com/p/quite-rss/wiki/runAplication"));
  connect(&page, SIGNAL(loadFinished(bool)),
           this, SLOT(renderStatistics()));

}

void UpdateAppDialog::closeDialog()
{
  settings_->setValue("remindAboutVersion", remindAboutVersion_->isChecked());
  settings_->setValue("updateAppDlg/geometry", saveGeometry());
}

void UpdateAppDialog::finishUpdatesChecking()
{
  reply_->deleteLater();

  QString info;
  QString newVersion = "";

  if (reply_->error() == QNetworkReply::NoError) {
    QString version = STRPRODUCTVER;
    QString date = STRDATE;

    QString str = QLatin1String(reply_->readAll());
    QString curVersion = str.section('"', 5, 5).section('\\', 0, 0);
    QString curDate = str.section('"', 3, 3).section('\\', 0, 0);

    if (version.contains(curVersion)) {
      str =
          "<a><font color=#4b4b4b>" +
          tr("You already have the latest version") +
          "</a>";
    } else {
      str =
          "<a><font color=#FF8040>" +
          tr("A new version of QuiteRSS is available!") + "</a>"
          "<p>" + QString("<a href=\"%1\">%2</a>").
          arg("http://code.google.com/p/quite-rss/downloads/list").
          arg(tr("Click here to go to the download page"));
      newVersion = curVersion;
    }
    info =
        "<html><style>a { color: blue; text-decoration: none; }</style><body>"
        "<p>" + tr("Your version is:") + " " +
        "<B>" + version + "</B>" + QString(" (%1)").arg(date) +
        "<BR>" + tr("Current version is:") + " " +
        "<B>" + curVersion + "</B>" + QString(" (%1)").arg(curDate) +
        "<p>" + str +
        "</body></html>";
  } else {
    //    qDebug() << reply_->error() << reply_->errorString();
    info = tr("Error checking updates");
    if (showDialog_)
      history_->setText("");
  }

  bool remind = settings_->value("remindAboutVersion", false).toBool();
  QString currentVersion = settings_->value("currentVersionApp", "").toString();
  if (!showDialog_) {
    if (!newVersion.isEmpty()) {
      if (currentVersion != newVersion) {
        settings_->setValue("currentVersionApp", newVersion);
        settings_->setValue("remindAboutVersion", false);
      } else if (remind) {
          newVersion = "";
      }
    }

    emit signalNewVersion(newVersion);
  } else {
    infoLabel->setText(info);

#if defined(Q_OS_WIN)
    if (QFile::exists(QCoreApplication::applicationDirPath() + "/Updater.exe") &&
        !newVersion.isEmpty())
      installButton_->show();
#endif

    if (!newVersion.isEmpty()) {
      if (currentVersion != newVersion) {
        settings_->setValue("currentVersionApp", newVersion);
        settings_->setValue("remindAboutVersion", false);
        remindAboutVersion_->setChecked(false);
        remindAboutVersion_->show();
      } else if (!remind) {
          remindAboutVersion_->show();
      }
    }
  }
}

void UpdateAppDialog::slotFinishHistoryReply()
{
  historyReply_->deleteLater();
  if (historyReply_->error() != QNetworkReply::NoError) {
    qDebug() << "error retrieving " << historyReply_->url();
    return;
  }

  QString str = QString::fromUtf8(historyReply_->readAll());

  history_->setHtml(str);
}

void UpdateAppDialog::updaterRun()
{
  close();
#if defined(Q_WS_WIN)
  QString updaterFile = QCoreApplication::applicationDirPath() + "/Updater.exe";
  ShellExecute(0, 0, (wchar_t *)updaterFile.utf16(), 0, 0, SW_SHOWNORMAL);
#endif
}

void UpdateAppDialog::renderStatistics()
{
  reply_ = manager_.get(QNetworkRequest(QUrl("http://quite-rss.googlecode.com/hg/src/VersionNo.h")));
  connect(reply_, SIGNAL(finished()), this, SLOT(finishUpdatesChecking()));
}
