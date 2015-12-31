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
message(Qt is installed in $$[QT_INSTALL_PREFIX])
        QT += quick multimedia network
        DEFINES+=QT5
}


#uri = DceScreenSaver
#URI = DceScreenSaver #$$TARGET#$$replace(TARGETPATH, "/", ".")
#QMAKE_MOC_OPTIONS += -Muri=$$URI

qmldir.files = qmldir

linux-g++{
    DESTDIR=../../imports/DceScreenSaver
}

linux-rasp-pi-g++{
    DESTDIR=../../imports/DceScreenSaver
    DEFINES+=RPI
    RASP_INSTALL_TARGET=/opt/qt5-rpi/qml/DceScreenSaver
}

macx-clang{
message( Building for OS x )
DESTDIR=../../imports/DceScreenSaver#$$[QT_INSTALL_IMPORTS]/DceScreenSaver
QMLDIR_TARGET = $$DESTDIR

}

android-g++{

    contains(QT_VERSION,5.*.*){
    DESTDIR=$$PWD/../../../platforms/Android/androidPlugins/Qt5/armeabi-v7a/  #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.
    } else {
    DESTDIR=$$PWD/../../../platforms/Android/androidPlugins/armeabi-v7a/  #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.
    }
}

macx-ios-clang{
message("Building static for iOS")
uri = DceScreenSaver
URI = DceScreenSaver #$$TARGET#$$replace(TARGETPATH, "/", ".")
QMAKE_MOC_OPTIONS = -Muri=DceScreenSaver

TARGET = dcescreensaverplugin
TARGET = $$qtLibraryTarget($$TARGET)
QMAKE_CXXFLAGS+=-Wno-c++11-narrowing
DESTDIR = $$[QT_INSTALL_IMPORTS]/DceScreenSaver
QMLDIR_TARGET=$$DESTDIR
CONFIG+=static
QMAKE_POST_LINK= $${QMAKE_COPY} $${_PRO_FILE_PWD_}/qmldir $${DESTDIR}$$escape_expand(\n\t)
}

# Input
SOURCES += \
    dcescreensaver_plugin.cpp \
    dcescreensaver.cpp

HEADERS += \
    dcescreensaver_plugin.h \
    dcescreensaver.h

OTHER_FILES = qmldir \
    android/res/values-es/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-ms/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-de/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-ru/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-mdpi/icon.png \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/version.xml \
    android/AndroidManifest.xml

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {

android-g++{


    QT5{
    QMLDIR_TARGET=../../../platforms/Android/androidComponents/Qt5/DceScreenSaver/qmldir
    } else{
    QMLDIR_TARGET=../../../platforms/Android/androidComponents/DceScreenSaver/qmldir
    }
}

linux-g++{
   QMLDIR_TARGET=$$DESTDIR
}

linux-rasp-pi-g++{
QMLDIR_TARGET=$$DESTDIR
}



macx-g++{

QMLDIR_TARGET = $$DESTDIR

}

DEPLOYMENTFOLDERS+=qmldir

copy_qmldir.target=$$OUT_PWD
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}



unix {
    maemo5 | !isEmpty(MEEGO_VERSION_MAJOR) {
       installPath = /usr/lib/qt4/imports/$$replace(uri, \\., /)
    } else {
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
message("qmldir target path at" $$QMLDIR_TARGET)
message("$$URI")

