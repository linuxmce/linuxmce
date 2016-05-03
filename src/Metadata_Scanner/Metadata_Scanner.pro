#-------------------------------------------------
#
# Project created by QtCreator 2016-04-03T23:52:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Metadata_Scanner
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    scanmanager.cpp \
    items/mediaitem.cpp  \
    parsing/mediaparser.cpp \
    services/metadataservicebase.cpp \
    databaseinteractor.cpp

HEADERS += \
    scanmanager.h \
    items/mediaitem.h \
    parsing/mediaparser.h \
    services/metadataservicebase.h \
    databaseinteractor.h
