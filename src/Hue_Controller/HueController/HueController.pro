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

include (../../QtCommonIncludes/PlutoUtils.pri)
SOURCES += ../Main.cpp \
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
    ../../DCE/ClientSocket.cpp \
    ../../DCE/DeviceData_Base.cpp \
    ../../DCE/DeviceData_Impl.cpp \
    ../../Gen_Devices/HueControllerBase.cpp \
    ../HueController.cpp \
    huecontrollerhardware.cpp \
    ../huecommand.cpp \
    abstractwirelessbulb.cpp \
    huemotionsensor.cpp \
    huedaynightsensor.cpp

HEADERS += \
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
    ../../pluto_main/Define_DeviceCategory.h \
    ../../pluto_main/Define_DeviceTemplate.h \
    ../../Gen_Devices/HueControllerBase.h \
    ../HueController.h \
    huecontrollerhardware.h \
    ../huecommand.h \
    abstractwirelessbulb.h \
    huemotionsensor.h \
    huedaynightsensor.h
