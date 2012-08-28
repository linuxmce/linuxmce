#-------------------------------------------------
#
# Project created by QtCreator 2012-08-24T23:00:22
#
#-------------------------------------------------

QT       += network opengl sql xml phonon multimedia

QT       -= gui

INCLUDEPATH += ../../ ../../DCE/ ../../Gen_Devices



TARGET = qMediaPlayer_Plugin
TEMPLATE = lib
CONFIG += console

DEFINES += QTMEDIAPLAYERPLUGIN_LIBRARY

SOURCES += \
    ../qMediaPlayer_Plugin.cpp \
    ../../Gen_Devices/qMediaPlayer_PluginBase.cpp \
    ../Main.cpp \
    ../qmediastream.cpp

HEADERS +=\
    ../qMediaPlayer_Plugin.h \
    ../../Gen_Devices/qMediaPlayer_PluginBase.h \
    ../qmediastream.h

unix:!macx:!symbian: LIBS += -L$$PWD/../../lib/ -lDCECommon
unix:!macx:!symbian: LIBS += -L$$PWD/../../lib/ -lpluto_main
unix:!macx:!symbian: LIBS += -L$$PWD/../../lib/ -lPlutoUtils
unix:!macx:!symbian: LIBS += -L$$PWD/../../lib/ -lSerializeClass
unix:!macx:!symbian: LIBS += -L$$PWD/../../lib/ -lMessageTranslation
unix:!macx:!symbian: LIBS += -L$$PWD/../../lib/ -lSerial

INCLUDEPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../



symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE0ADD22D
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = qtMediaPlayerPlugin.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/pluto/lib
    }
    INSTALLS += target
}
