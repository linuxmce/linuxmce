#-------------------------------------------------
#
# Project created by QtCreator 2011-06-01T17:18:03
#
#-------------------------------------------------

QT       += core gui += webkit
INCLUDEPATH += ../../../ ../../../DCE/

LIBS += -L/usr/pluto/lib/ -lDCECommon -lSerializeClass -lPlutoUtils -lpluto_main -lSerial

TARGET = lmceBrowser
TEMPLATE = app


SOURCES += main.cpp\
        browserwindow.cpp \
    ../../DCE_Browser.cpp \
    ../../../Gen_Devices/DCE_BrowserBase.cpp

HEADERS  += browserwindow.h \
    ../../DCE_Browser.h \
    ../../../Gen_Devices/DCE_BrowserBase.h
