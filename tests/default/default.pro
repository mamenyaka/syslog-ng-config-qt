TEMPLATE = app
CONFIG += c++14 testcase
TARGET = default
INCLUDEPATH += ../../src
MOC_DIR = ../build/moc
OBJECTS_DIR = ../build/obj
QT += widgets testlib
LIBS += -lyaml-cpp ../../build/obj/dialog.o ../../build/obj/option.o ../../build/obj/object.o ../../build/obj/config.o

SOURCES += default.cpp

HEADERS += \
    default.h \
    ../../src/dialog.h

