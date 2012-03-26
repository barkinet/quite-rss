#include <QtGui>

#include <qtsingleapplication.h>
#include "rsslisting.h"

QSplashScreen *splash;

void loadModules(QSplashScreen* psplash)
{
  QElapsedTimer time;
  time.start();

  QProgressBar splashProgress;
  splashProgress.setObjectName("splashProgress");
  splashProgress.setTextVisible(false);
  splashProgress.setFixedHeight(10);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addStretch(1);
  layout->addWidget(&splashProgress);
  splash->setLayout(layout);
  for (int i = 0; i < 100; ) {
    if (time.elapsed() >= 1) {
      time.start();
      ++i;
      qApp->processEvents();
      splashProgress.setValue(i);
      psplash->showMessage("Loading: " + QString::number(i) + "%",
                           Qt::AlignRight | Qt::AlignTop, Qt::white);
    }
  }
}

void createSplashScreen()
{
  splash = new QSplashScreen(QPixmap(":/images/images/splashScreen.png"));
  splash->setFixedSize(splash->pixmap().width(), splash->pixmap().height());
  splash->setContentsMargins(15, 0, 15, 0);
  splash->setEnabled(false);
  splash->showMessage("Prepare loading...",
                      Qt::AlignRight | Qt::AlignTop, Qt::white);
  splash->show();
}

int main(int argc, char **argv)
{
  QtSingleApplication app(argc, argv);
  if (app.isRunning()) {
    if (1 == argc) {
      app.sendMessage("--show");
    }
    else {
      QString message = app.arguments().value(1);
      for (int i = 2; i < argc; ++i)
        message += '\n' + app.arguments().value(i);
      app.sendMessage(message);
    }
    return 0;
  }

  createSplashScreen();

  app.setApplicationName("QuiteRss");
  app.setOrganizationName("QuiteRss");
  app.setWindowIcon(QIcon(":/images/images/QuiteRSS.ico"));
  app.setQuitOnLastWindowClosed(false);

  QString fileString = ":/style/qstyle";
  //    QString fileString = app.applicationDirPath() + "/Style/QuiteRSS.qss";
  QFile file(fileString);
  file.open(QFile::ReadOnly);
  app.setStyleSheet(QLatin1String(file.readAll()));

  RSSListing rsslisting;

  app.setActivationWindow(&rsslisting, true);
  QObject::connect(&app, SIGNAL(messageReceived(const QString&)),
                   &rsslisting, SLOT(receiveMessage(const QString&)));

  loadModules(splash);

  if (!rsslisting.startingTray_)
    rsslisting.show();
  rsslisting.traySystem->show();

  splash->finish(&rsslisting);
  rsslisting.setCurrentFeed();

  return app.exec();
}
