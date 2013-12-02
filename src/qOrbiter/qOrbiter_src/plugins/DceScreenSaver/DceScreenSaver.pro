TEMPLATE = lib
TARGET = DceScreenSaver

CONFIG += qt plugin

contains(QT_VERSION,4.*.*){

message("$$QT_VERSION DCE-ScreenSaver-Plugin")
        QT += declarative opengl
        DEFINES+=QT4
}

contains(QT_VERSION,5.*.*){
        message("$$QT_VERSION DCE-ScreenSaver-Plugin")
        QT += quick multimedia network
        DEFINES+=QT5
}


uri = DceScreenSaver
TARGET = $$qtLibraryTarget($$TARGET)

linux-g++{
DESTDIR=../../imports/DceScreenSaver
}
linux-rasp-pi-g++{
DESTDIR=../../imports/DceScreenSaver
DEFINES+=RPI
RASP_INSTALL_TARGET=/opt/qt5.2-rpi/qml #$$[QT_INSTALL_PREFIX]/qml
}


android-g++{

QT5{

DESTDIR=../../../platforms/Android/androidPlugins/Qt5/armeabi-v7a/  #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.
    } else {
DESTDIR=../../../platforms/Android/androidPlugins/armeabi-v7a/  #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.
    }
}


# Input
SOURCES += \
    dcescreensaver_plugin.cpp \
    dcescreensaver.cpp

HEADERS += \
    dcescreensaver_plugin.h \
    dcescreensaver.h

OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$DESTDIR) {

android-g++{
   QMLDIR_TARGET=../../../platforms/Android/androidComponents/Qt5/DceScreenSaver/qmldir
    QT5{
    } else{
    QMLDIR_TARGET=../../../platforms/Android/androidComponents/DceScreenSaver/qmldir
    }
}

linux-g++{
   QMLDIR_TARGET=$DESTDIR
}

linux-rasp-pi-g++{
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
    } else {
        installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
    }
    linux-rasp-pi-g++{
    installPath=/opt/QOrbiter/imports/$$replace(uri, \\., /) #$$RASP_INSTALL_TARGET/$$replace(uri, \\., /)
}
    qmldir.path = $$installPath
    target.path = $$installPath
   INSTALLS += target qmldir
}

