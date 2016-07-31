QT += core
QT -= gui

CONFIG += c++11

TARGET = construction
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    core_other.cpp \
    src/core/core_other.cpp \
    src/core/core_scope.cpp

HEADERS += \
    core.h \
    constr_num.h \
    src/core/core.h \
    src/num/constr_num.h
