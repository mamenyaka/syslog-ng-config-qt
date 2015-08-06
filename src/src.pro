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
    tab.cpp \
    scene.cpp \
    icon.cpp \
    dialog.cpp \
    option.cpp \
    driver.cpp \
    config.cpp

HEADERS += \
    mainwindow.h \
    tab.h \
    scene.h \
    icon.h \
    dialog.h \
    option.h \
    driver.h \
    config.h

FORMS += \
    mainwindow.ui \
    dialog.ui
