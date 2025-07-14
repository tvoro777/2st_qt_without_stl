QT += core gui widgets

CONFIG += c++17

TARGET = benchmarks

SOURCES += \
    source/main.cpp \
    source/gui/mainwindow.cpp \
    source/opener_filter/opener_filter.cpp

HEADERS += \
    source/gui/mainwindow.h \
    source/opener_filter/opener_filter.h



