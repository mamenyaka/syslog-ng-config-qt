QT += widgets testlib
TARGET = test
TEMPLATE = app
CONFIG += c++11
LIBS += -lyaml-cpp

SOURCES += test.cpp ../config.cpp

HEADERS += test.h ../config.h
