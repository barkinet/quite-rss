QT += core gui network xml webkit sql

TARGET = QtRss
TEMPLATE = app

HEADERS += rsslisting.h \
    VersionNo.h
SOURCES += main.cpp rsslisting.cpp

RESOURCES += \
    qtrss.qrc
RC_FILE = QtRssApp.rc

CODECFORTR  = UTF-8
CODECFORSRC = UTF-8
include(lang/lang.pri)
