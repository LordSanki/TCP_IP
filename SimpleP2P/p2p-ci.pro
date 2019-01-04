#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T19:47:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += -g -O0
TARGET = p2p-ci
TEMPLATE = app

DESTDIR = ../bin/

SOURCES += src/main.cpp\
        editline/editline.c \
        editline/sysunix.c \
        editline/complete.c \
        ui/mainwindow.cpp \
        src/server.cpp \
        src/client.cpp \
        src/request.cpp \
        src/response.cpp \
        src/method.cpp \
        src/ServerSocket.cpp \
        src/ClientSocket.cpp \
        src/Socket.cpp \
        src/addmethod.cpp \
        src/lookupmethod.cpp \
        src/listmethod.cpp \
        src/getmethod.cpp \
        src/common.cpp


INCLUDEPATH += include editline

HEADERS  += ui/mainwindow.h \
            include/server.h \
            include/client.h \
            include/addmethod.h \
            include/lookupmethod.h \
            include/listmethod.h \
            include/getmethod.h

FORMS    += ui/mainwindow.ui

