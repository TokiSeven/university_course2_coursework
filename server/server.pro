QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = server

TEMPLATE = app

CONFIG -= console

SOURCES += main.cpp \
    server.cpp \
    network_main.cpp \
    dialog.cpp

HEADERS += \
    server.h \
    ../game/data.h \
    network_main.h \
    dialog.h

FORMS += \
    dialog.ui
