TEMPLATE = lib
# This does not seem to do anything
#TARGET = AudioVisual

contains(QT_VERSION,4.*.*){

message("$$QT_VERSION DCE-Av-Plugin")
	QT += declarative phonon network
	DEFINES+=QT4
}

contains(QT_VERSION,5.0.*){
	message("$$QT_VERSION DCE-Av-Plugin")
        QT += quick1 multimedia network
	DEFINES+=QT5
}

CONFIG += qt plugin
CONFIG += warn_off
QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID
INCLUDEPATH += ../../../../ ../../../../DCE/ $$[QT_INSTALL_PREFIX]/include/phonon/phonon

TARGET = $$qtLibraryTarget($$TARGET)

DESTDIR=../../imports/AudioVisual

uri = AudioVisual
# Input
SOURCES += \
	audiovisual_plugin.cpp \   
	colorfilterproxywidget.cpp \
	mediamanager.cpp \
	../../../../qMediaPlayer/qMediaPlayer.cpp \
	../../../../PlutoUtils/uuencode.cpp \
	../../../../PlutoUtils/ThreadedClass.cpp \
	../../../../PlutoUtils/Other.cpp \
	../../../../PlutoUtils/MultiThreadIncludes.cpp \
	../../../../PlutoUtils/minilzo.cpp \
	../../../../PlutoUtils/md5c.cpp \
	../../../../PlutoUtils/FileUtils.cpp \
	../../../../PlutoUtils/CommonIncludes.cpp \
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
	../../../../PlutoUtils/StringUtils.cpp \
	../../../../DCE/ClientSocket.cpp \
	../../../../DCE/DeviceData_Base.cpp \
	../../../../DCE/DeviceData_Impl.cpp \
	../../../../PlutoUtils/getch.cpp \
	../../../../Gen_Devices/qMediaPlayerBase.cpp \
	../../../../Gen_Devices/qOrbiterBase.cpp
	
HEADERS += \
	audiovisual_plugin.h \  
	colorfilterproxywidget.h \
	mediamanager.h \
	../../../../qMediaPlayer/qMediaPlayer.h \
	../../../../PlutoUtils/ThreadedClass.h \
	../../../../PlutoUtils/MultiThreadIncludes.h \
	../../../../PlutoUtils/StringUtils.h \
	../../../../PlutoUtils/CommonIncludes.h \
	../../../../PlutoUtils/Other.h \
	../../../../PlutoUtils/getch.h \
	../../../../PlutoUtils/MyStl.h \
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
	../../../../PlutoUtils/FileUtils.h \
	../../../../pluto_main/Define_DeviceCategory.h \
	../../../../pluto_main/Define_DeviceTemplate.h \
	../../../../Gen_Devices/qMediaPlayerBase.h \
	../../../../Gen_Devices/qOrbiterBase.h

OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
	copy_qmldir.target = $$OUT_PWD/qmldir
	copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
	copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
	QMAKE_EXTRA_TARGETS += copy_qmldir
	PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir

symbian {
	TARGET.EPOCALLOWDLLDATA = 1
} else:unix {
	maemo5 | !isEmpty(MEEGO_VERSION_MAJOR) {
		installPath = /usr/lib/qt4/imports/$$replace(uri, \\., /)
	} else {
		installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
	}
	qmldir.path = $$installPath
	target.path = $$installPath
	INSTALLS += target qmldir
}
