TEMPLATE = lib
TARGET = DceScreenSaver

CONFIG += qt plugin

contains(QT_VERSION,4.*.*){

message("$$QT_VERSION DCE-ScreenSaver-Plugin")
        QT += declarative opengl
        DEFINES+=QT4
}

contains(QT_VERSION,5.0.*){
        message("$$QT_VERSION DCE-ScreenSaver-Plugin")
        QT += quick1 multimedia network
        DEFINES+=QT5
}

uri = DceScreenSaver
TARGET = $$qtLibraryTarget($$TARGET)

linux-g++{
DESTDIR=../../imports/DceScreenSaver
}

android-g++{
DESTDIR=../../../platforms/Android/androidPlugins/armeabi-v7a/  #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.
}


# Input
SOURCES += \
    dcescreensaver_plugin.cpp \
    dcescreensaver.cpp

HEADERS += \
    dcescreensaver_plugin.h \
    dcescreensaver.h

OTHER_FILES = qmldir

android-g++{
OTHER_FILES+= android/res/values-rs/strings.xml \
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
}

!equals(_PRO_FILE_PWD_, $$DESTDIR) {

QMLDIR_TARGET=../../../platforms/Android/androidComponents/DceScreenSaver/qmldir
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
    } else {
        installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
    }

    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

