TEMPLATE = app
CONFIG += c++14 testcase
TARGET = sources
INCLUDEPATH += ../../src
MOC_DIR = ../build/moc
OBJECTS_DIR = ../build/obj
QT += widgets testlib
LIBS += -lyaml-cpp ../../build/obj/dialog.o ../../build/obj/option.o ../../build/obj/object.o ../../build/obj/config.o

SOURCES += sources.cpp

HEADERS += \
    sources.h \
    ../../src/dialog.h

