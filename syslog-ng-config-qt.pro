QT += core gui widgets webkitwidgets

TARGET = syslog-ng-config-qt
TEMPLATE = app
CONFIG += c++11
LIBS += -lyaml-cpp -lboost_system -lboost_filesystem

SOURCES += main.cpp mainwindow.cpp widget.cpp dialog.cpp config.cpp

HEADERS += mainwindow.h widget.h dialog.h config.h

FORMS += mainwindow.ui dialog.ui
