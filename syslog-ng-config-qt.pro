QT += core gui widgets

TARGET = syslog-ng-config-qt
TEMPLATE = app
CONFIG += c++14
LIBS += -lyaml-cpp

SOURCES += main.cpp mainwindow.cpp icon.cpp drivers.cpp scene.cpp dialog.cpp config.cpp

HEADERS += mainwindow.h icon.h drivers.h scene.h dialog.h config.h

FORMS += mainwindow.ui dialog.ui
