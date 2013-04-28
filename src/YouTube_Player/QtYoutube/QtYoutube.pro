#-------------------------------------------------
#
# Project created by QtCreator 2013-04-27T19:20:36
#
#-------------------------------------------------

QT       += core gui webkitwidgets network

INCLUDEPATH += ../../ ../../DCE/

LIBS += -L/usr/pluto/lib -lDCECommon -lSerializeClass -lPlutoUtils -lpluto_main

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtYoutube
TEMPLATE = app


SOURCES += main.cpp\
        youtubewindow.cpp \
        ../YouTube_Player.cpp \
        ../../Gen_Devices/YouTube_PlayerBase.cpp

HEADERS  += youtubewindow.h \
            ../YouTube_Player.h \
            ../../Gen_Devices/YouTube_PlayerBase.h
