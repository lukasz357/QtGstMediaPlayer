#-------------------------------------------------
#
# Project created by QtCreator 2014-01-04T12:45:01
#
#-------------------------------------------------

TARGET = QtGstMediaPlayer
TEMPLATE = app

# produce nice compilation output
#CONFIG += silent

# Tell qmake to use pkg-config to find QtGStreamer.
CONFIG += link_pkgconfig

QT += core gui

contains(QT_VERSION, ^4\\..*) {
  PKGCONFIG += QtGStreamer-0.10 QtGStreamerUi-0.10
}
contains(QT_VERSION, ^5\\..*) {
  PKGCONFIG += Qt5GStreamer-0.10 Qt5GStreamerUi-0.10
  QT += widgets
}

DEFINES += QT_NO_EMIT

SOURCES += main.cpp\
           mainwindow.cpp \
           player.cpp

HEADERS  += mainwindow.h \
            player.h

FORMS    += mainwindow.ui
