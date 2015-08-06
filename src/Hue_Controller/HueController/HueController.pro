# -------------------------------------------------
# Project created by QtCreator 2013-05-04T14:36:21
# -------------------------------------------------
QT       += core network gui


TARGET = HueController
target.path = /tmp #development option with ssh.
INSTALLS = target
CONFIG   +=sharedlib
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
INCLUDEPATH += /usr/include/qjson/ \
    ../../ \
    ../../DCE/

LIBS += -lqjson
DESTDIR=../../bin
SOURCES += ../Main.cpp \
    ../../PlutoUtils/uuencode.cpp \
    ../../PlutoUtils/ThreadedClass.cpp \
    ../../PlutoUtils/Other.cpp \
    ../../PlutoUtils/MultiThreadIncludes.cpp \
    ../../PlutoUtils/minilzo.cpp \
    ../../PlutoUtils/md5c.cpp \
    ../../PlutoUtils/FileUtils.cpp \
    ../../PlutoUtils/CommonIncludes.cpp \
    ../../SerializeClass/SerializeClass.cpp \
    ../../DCE/Virtual_Device_Translator.cpp \
    ../../DCE/Socket.cpp \
    ../../DCE/ServerLogger.cpp \
    ../../DCE/PlainClientSocket.cpp \
    ../../DCE/MessageBuffer.cpp \
    ../../DCE/Message.cpp \
    ../../DCE/HandleRequestSocket.cpp \
    ../../DCE/Logger.cpp \
    ../../DCE/Event_Impl.cpp \
    ../../DCE/DCEConfig.cpp \
    ../../DCE/DataGrid.cpp \
    ../../DCE/Command_Impl.cpp \
    ../../DCE/AlarmManager.cpp \
    ../../PlutoUtils/StringUtils.cpp \
    ../../DCE/ClientSocket.cpp \
    ../../DCE/DeviceData_Base.cpp \
    ../../DCE/DeviceData_Impl.cpp \
    ../../PlutoUtils/getch.cpp \
    ../../Gen_Devices/HueControllerBase.cpp \
    ../HueController.cpp \
    huecontrollerhardware.cpp \
    huebulb.cpp \
    ../huecommand.cpp \
    abstractwirelessbulb.cpp \
    hueluxbulb.cpp

HEADERS += \
     ../../PlutoUtils/ThreadedClass.h \
    ../../PlutoUtils/MultiThreadIncludes.h \
    ../../PlutoUtils/StringUtils.h \
    ../../PlutoUtils/CommonIncludes.h \
    ../../PlutoUtils/Other.h \
    ../../PlutoUtils/getch.h \
    ../../PlutoUtils/MyStl.h \
    ../../DCE/DeviceData_Base.h \
    ../../DCE/Message.h \
    ../../DCE/ServerLogger.h \
    ../../DCE/Logger.h \
    ../../DCE/Virtual_Device_Translator.h \
    ../../DCE/PlutoLockLogger.h \
    ../../DCE/ClientSocket.h \
    ../../DCE/PlainClientSocket.h \
    ../../DCE/AlarmManager.h \
    ../../SerializeClass/SerializeClass.h \
    ../../PlutoUtils/FileUtils.h \
    ../../pluto_main/Define_DeviceCategory.h \
    ../../pluto_main/Define_DeviceTemplate.h \
    ../../Gen_Devices/HueControllerBase.h \
    ../HueController.h \
    huecontrollerhardware.h \
    huebulb.h \
    ../huecommand.h \
    abstractwirelessbulb.h \
    hueluxbulb.h
