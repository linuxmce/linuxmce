TEMPLATE = lib
TARGET = AudioVisual

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
    RASP_INSTALL_TARGET=/opt/qt5-rpi/qml/
}

linux-rasp-pi-g++{
    DESTDIR=../../imports/AudioVisual
    DEFINES+=RPI
    RASP_INSTALL_TARGET=/opt/qt5-rpi/qml/
}

android-g++{
    DESTDIR=$$PWD/../../../platforms/Android/androidPlugins/Qt5/armeabi-v7a/  #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.
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

OTHER_FILES = qmldir

# Input
SOURCES += \
	audiovisual_plugin.cpp \   
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




!equals(_PRO_FILE_PWD_, $$DESTDIR) {

android-g++{
   QMLDIR_TARGET=../../../platforms/Android/androidComponents/AudioVisual/qmldir
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



