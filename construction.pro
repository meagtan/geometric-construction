QT += core
QT -= gui

CONFIG += c++11

TARGET = construction
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    src/core/core_other.cpp \
    src/core/core_scope.cpp \
    src/main.cpp \
    src/core/geom.cpp \
    src/core/constructor.cpp \
    src/core/constr_num.cpp \
    test/constr_num_test.cpp

HEADERS += \
    include/core.h \
    include/constr_num.h \
    include/geom.h
