#-------------------------------------------------
#
# Project created by QtCreator 2015-06-23T19:04:13
#
#-------------------------------------------------

#Qt Modules
QT       += core network
QT       -= gui


#Qt app configuration
TARGET = Cloud_Interface
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#includes
INCLUDEPATH += /usr/include/qjson/ \
    ../../ \
    ../../DCE/

#additional libs - yes im still using qjson
LIBS+=-lqjson

#Destination, for dce build env
DESTDIR=../../bin

include (../../QtCommonIncludes/PlutoUtils.pri)

#DCE headers
HEADERS+= \
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
    ../../Gen_Devices/Cloud_InterfaceBase.h \
    ../Cloud_Interface.h \
    cloudinterfacecotroller.h \
    cloudsettings.h



#DCE-specific sources
SOURCES+= \
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
    ../../Gen_Devices/Cloud_InterfaceBase.cpp \
     ../Cloud_Interface.cpp \
    cloudinterfacecotroller.cpp \
    cloudsettings.cpp

#Qt Sources
SOURCES += ../Main.cpp
