QT += core network
QT -= gui

TARGET = lmce-metadata-scanner
CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    filenameparser.cpp \
    metadatamanager.cpp \
    linuxmcefile.cpp \
    networkmetadatamanager.cpp \
    databaseinterface.cpp

HEADERS += \
    filenameparser.h \
    metadatamanager.h \
    linuxmcefile.h \
    networkmetadatamanager.h \
    databaseinterface.h

