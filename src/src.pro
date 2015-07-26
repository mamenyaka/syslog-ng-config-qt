TEMPLATE = app
CONFIG += c++14
TARGET = syslog-ng-config-qt
DESTDIR = ../
MOC_DIR = ../build/moc
OBJECTS_DIR = ../build/obj
QT += core gui widgets
LIBS += -lyaml-cpp

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    icon.cpp \
    drivers.cpp \
    scene.cpp \
    dialog.cpp \
    config.cpp

HEADERS += \
    mainwindow.h \
    icon.h \
    drivers.h \
    scene.h \
    dialog.h \
    config.h

FORMS += \
    mainwindow.ui \
    dialog.ui
