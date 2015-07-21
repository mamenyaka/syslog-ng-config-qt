QT += widgets testlib
TARGET = test
TEMPLATE = app
CONFIG += c++14
LIBS += -lyaml-cpp

SOURCES += test.cpp ../config.cpp

HEADERS += test.h
