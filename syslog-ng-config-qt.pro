QT += core gui widgets

TARGET = syslog-ng-config-qt
TEMPLATE = app
CONFIG += c++11
LIBS += -lyaml-cpp

SOURCES += main.cpp mainwindow.cpp widget.cpp dialog.cpp config.cpp

HEADERS += mainwindow.h widget.h dialog.h config.h

FORMS += mainwindow.ui
