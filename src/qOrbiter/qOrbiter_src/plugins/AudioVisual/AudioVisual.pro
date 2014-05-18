TEMPLATE = lib
TARGET = AudioVisual

CONFIG += qt plugin
CONFIG += warn_off
QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID
INCLUDEPATH += ../../../../ ../../../../DCE/


contains(QT_VERSION,4.*.*){

         message("$$QT_VERSION DCE-Av-Plugin")
         QT += declarative phonon network opengl

        android-g++{
       QT-=phonon
        message("For Android")
         }

	DEFINES+=QT4
}

contains(QT_VERSION,5.*.*){
	message("$$QT_VERSION DCE-Av-Plugin")
        QT += quick multimedia network opengl core
        DEFINES+=QT5
}


uri = AudioVisual
TARGET = $$qtLibraryTarget($$TARGET)



linux-g++{

    !RPI{
    INCLUDEPATH+=$$[QT_INSTALL_PREFIX]/include/phonon/Phonon
    DESTDIR=../../imports/AudioVisual
    }

    RPI{
    DESTDIR=../../imports/AudioVisual
    RASP_INSTALL_TARGET=/opt/qt5.2-rpi/qml #$$[QT_INSTALL_PREFIX]/qml
    }
}

linux-rasp-pi-g++{
    DESTDIR=../../imports/AudioVisual
    DEFINES+=RPI
    RASP_INSTALL_TARGET=/opt/qt5.2-rpi/qml #$$[QT_INSTALL_PREFIX]/qml
}

android-g++{

    contains(QT_VERSION,5.*.*){
    DESTDIR=../../../platforms/Android/androidPlugins/Qt5/armeabi-v7a/  #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.
    } else {
    DESTDIR=../../../platforms/Android/androidPlugins/armeabi-v7a/  #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.
    }
}

macx-clang{
    DESTDIR=../../imports/AudioVisual
}

OTHER_FILES = qmldir

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


!equals(_PRO_FILE_PWD_, $$DESTDIR) {

android-g++{
   QMLDIR_TARGET=../../../platforms/Android/androidComponents/Qt5/AudioVisual/qmldir
    QT5{
    } else{
    QMLDIR_TARGET=../../../platforms/Android/androidComponents/AudioVisual/qmldir
    }
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
    installPath=/opt/qt5.2-rpi/qml/$$replace(uri, \\., /) #$$RASP_INSTALL_TARGET/$$replace(uri, \\., /)
}
	qmldir.path = $$installPath
	target.path = $$installPath
        INSTALLS += target qmldir
}

message("Plugin install path at" $$DESTDIR)

OTHER_FILES += \
    android/res/values-rs/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-it/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values/strings.xml \
    android/res/values-et/strings.xml \
    android/version.xml \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/AndroidManifest.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-mdpi/icon.png \
    android/res/values/libs.xml \
    android/res/drawable-hdpi/icon.png

