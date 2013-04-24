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
#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QtGui>

class SplashScreen : public QSplashScreen
{
  Q_OBJECT
private:
  QProgressBar splashProgress;

private slots:
  void slotSplashTimeOut();

public:
  explicit SplashScreen(const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0);

  void loadModules();
  QTimer *splashTimer_;

public slots:


signals:


};

#endif // SPLASHSCREEN_H
