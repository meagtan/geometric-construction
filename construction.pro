QT += core
QT -= gui

CONFIG += c++11

TARGET = construction
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    core_other.cpp \
    src/core/core_other.cpp \
    src/core/core_scope.cpp \
    src/main.cpp

HEADERS += \
    core.h \
    constr_num.h \
    src/core/core.h \
    src/num/constr_num.h
