QT += widgets testlib
TARGET = test
TEMPLATE = app
CONFIG += c++11
LIBS += -lyaml-cpp -lboost_system -lboost_filesystem

SOURCES += test.cpp ../config.cpp

HEADERS += test.h ../config.h
