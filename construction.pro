QT += core
QT -= gui

CONFIG += c++11

TARGET = construction
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    core_scope.cpp \
    core_other.cpp

HEADERS += \
    core.h \
    constr_num.h
