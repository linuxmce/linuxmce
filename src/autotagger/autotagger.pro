#-------------------------------------------------
#
# Project created by QtCreator 2010-10-05T09:55:16
#
#-------------------------------------------------

QT       += core
QT       += network
QT       += xml
QT       += sql

INCLUDEPATH = /usr/include/mysql/
TARGET = ../bin/linuxmceTag
target.path = /usr/pluto/bin
CONFIG   +=sharedlib
CONFIG   += console
CONFIG   -= app_bundle


TEMPLATE = app
SOURCES += main.cpp \
    autotagger.cpp \
    tvshows.cpp \
    films.cpp \
    databaseFunctions.cpp \
   # taglogger.cpp

HEADERS += \
    autotagger.h \
    pilot.h \
    film.h \
    databaseFunctions.h \
    taglogger.h

OTHER_FILES += \
    Readme.txt
INSTALLS = target
