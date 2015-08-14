TEMPLATE = app
CONFIG += c++14
TARGET = syslog-ng-config-qt
DESTDIR = ../
MOC_DIR = ../build/moc
OBJECTS_DIR = ../build/obj
QT += core gui widgets
LIBS += -lyaml-cpp

SOURCES += \
    option.cpp \
    object.cpp \
    config.cpp \
    icon.cpp \
    tab.cpp \
    dialog.cpp \
    scene.cpp \
    mainwindow.cpp \
    main.cpp

HEADERS += \
    option.h \
    object.h \
    config.h \
    icon.h \
    tab.h \
    dialog.h \
    scene.h \
    mainwindow.h

FORMS += \
    mainwindow.ui \
    dialog.ui
