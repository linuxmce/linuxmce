TEMPLATE = lib
TARGET = AudioVisual
CONFIG+= c++11
CONFIG += qt plugin console
CONFIG += warn_off
CONFIG -= android_install
 QT += quick multimedia network opengl core
 DEFINES+=QT5
QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID
INCLUDEPATH += ../../../../ ../../../../DCE/



uri = AudioVisual
URI = AudioVisual #$$TARGET
#TARGET = $$qtLibraryTarget($$TARGET)

linux-g++{

    !RPI{
    INCLUDEPATH+=$$[QT_INSTALL_PREFIX]/include/phonon/Phonon
    DESTDIR=../../imports/AudioVisual
    }
}

linux-rasp-pi-g++{
    DESTDIR=../../imports/AudioVisual
    DEFINES+=RPI
    RASP_INSTALL_TARGET=/usr/pluto/bin/imports/
    QT+= dbus

    SOURCES += \
    playerInterfaces/omxdbusplayerinterface.cpp \
    playerInterfaces/omxinterface.cpp \
    playerInterfaces/OmxDbusProxy.cpp

    HEADERS += \
    playerInterfaces/omxdbusplayerinterface.h \
    playerInterfaces/omxinterface.h \
    playerInterfaces/OmxDbusProxy.h
}

android-g++{
    DESTDIR=$$[QT_INSTALL_PREFIX]/qml/AudioVisual
  }

macx-g++{
message( Building for OS x )
DESTDIR=../../imports/AudioVisual #$$[QT_INSTALL_PREFIX]/qml
}

macx-ios-clang {
message("Building in static mode for iOS")
QMAKE_CXXFLAGS+=-Wno-c++11-narrowing
TARGET= audiovisualplugin
DESTDIR=$$[QT_INSTALL_IMPORTS]/AudioVisual
QMLDIR_TARGET=$$DESTDIR
CONFIG+=staticlib
QMAKE_MOC_OPTIONS += -Muri=$$URI


QMAKE_POST_LINK= $${QMAKE_COPY} $${_PRO_FILE_PWD_}/qmldir $${DESTDIR}$$escape_expand(\n\t)

}

include (../../../../QtCommonIncludes/PlutoUtils.pri)

OTHER_FILES = qmldir

# Input
SOURCES += \
        audiovisual_plugin.cpp \
        ../../../../qMediaPlayer/qMediaPlayer.cpp \
        ../../../../SerializeClass/SerializeClass.cpp \
        ../../../../DCE/Virtual_Device_Translator.cpp \
        ../../../../DCE/Socket.cpp \
        ../../../../DCE/DCEConfig.cpp \
        ../../../../DCE/ServerLogger.cpp \
        ../../../../DCE/PlainClientSocket.cpp \
        ../../../../DCE/MessageBuffer.cpp \
        ../../../..//DCE/Message.cpp \
        ../../../../DCE/HandleRequestSocket.cpp \
        ../../../../DCE/Logger.cpp \
        ../../../../DCE/Event_Impl.cpp \
        ../../../../DCE/DataGrid.cpp \
        ../../../../DCE/Command_Impl.cpp \
        ../../../../DCE/AlarmManager.cpp \
        ../../../../DCE/ClientSocket.cpp \
        ../../../../DCE/DeviceData_Base.cpp \
        ../../../../DCE/DeviceData_Impl.cpp \
	../../../../Gen_Devices/qMediaPlayerBase.cpp \
        ../../../../Gen_Devices/qOrbiterBase.cpp \
        mediabase/mediamanagerbase.cpp \
        playerInterfaces/defaultplayerinterface.cpp


	
HEADERS += \
        audiovisual_plugin.h \
        ../../../../qMediaPlayer/qMediaPlayer.h \
	../../../../DCE/DeviceData_Base.h \
	../../../../DCE/Message.h \
	../../../../DCE/ServerLogger.h \
	../../../../DCE/Logger.h \
	../../../../DCE/Virtual_Device_Translator.h \
	../../../../DCE/PlutoLockLogger.h \
	../../../../DCE/ClientSocket.h \
	../../../../DCE/PlainClientSocket.h \
	../../../../DCE/AlarmManager.h \
        ../../../../SerializeClass/SerializeClass.h \
	../../../../pluto_main/Define_DeviceCategory.h \
	../../../../pluto_main/Define_DeviceTemplate.h \
	../../../../Gen_Devices/qMediaPlayerBase.h \
        ../../../../Gen_Devices/qOrbiterBase.h \
        mediabase/mediamanagerbase.h \
        playerInterfaces/defaultplayerinterface.h






!equals(_PRO_FILE_PWD_, $$DESTDIR) {

android-g++{
   QMLDIR_TARGET=$$DESTDIR/qmldir
}

linux-g++{
   QMLDIR_TARGET=$DESTDIR
}

copy_qmldir.target=$$QMLDIR_TARGET
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
	maemo5 | !isEmpty(MEEGO_VERSION_MAJOR) {
		installPath = /usr/lib/qt4/imports/$$replace(uri, \\., /)
        } RPI {
                installPath = $$RASP_INSTALL_TARGET$$replace(uri, \\., /)
        }else {
		installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
        }

    linux-rasp-pi-g++{
    installPath=$$RASP_INSTALL_TARGET$$replace(uri, \\., /) #$$RASP_INSTALL_TARGET/$$replace(uri, \\., /)
}
	qmldir.path = $$installPath
	target.path = $$installPath
        INSTALLS += target qmldir
}

message("Plugin install path at" $$DESTDIR)



