QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = syslog-ng-config-qt
TEMPLATE = app
CONFIG += c++11
LIBS += -lyaml-cpp

SOURCES += main.cpp mainwindow.cpp widget.cpp dialog.cpp

HEADERS += mainwindow.h widget.h dialog.h driver.h option.h shape.h

FORMS += mainwindow.ui
