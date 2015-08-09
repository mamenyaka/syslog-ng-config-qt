TEMPLATE = app
CONFIG += c++14 testcase
TARGET = test
MOC_DIR = ../build/moc
OBJECTS_DIR = ../build/obj
QT += widgets testlib
LIBS += -lyaml-cpp

SOURCES += \
    test.cpp \
    ../src/dialog.cpp \
    ../src/option.cpp \
    ../src/object.cpp \
    ../src/config.cpp

HEADERS += \
    test.h \
    ../src/dialog.h
