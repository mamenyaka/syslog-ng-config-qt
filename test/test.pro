TEMPLATE = app
CONFIG += c++14 testcase
TARGET = test
MOC_DIR = ../build/moc
OBJECTS_DIR = ../build/obj
QT += widgets testlib
LIBS += -lyaml-cpp

SOURCES += \
    test.cpp \
    ../src/option.cpp \
    ../src/config.cpp

HEADERS += test.h
