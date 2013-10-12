#-------------------------------------------------
#
# Project created by QtCreator 2013-10-07T13:19:34
#
#-------------------------------------------------

QT       -= gui

TARGET = QtMediaCallbacks
TEMPLATE = lib

DEFINES += QTMEDIACALLBACKS_LIBRARY

SOURCES += qtmediacallbacks.cpp

HEADERS +=\
        QtMediaCallbacks_global.h \
    org_kde_necessitas_origo_MediaCallbackInterface.h

#DESTDIR=$$[QT_INSTALL_PREFIX]/lib
DESTDIR=../../../../platforms/Android/androidPlugins/armeabi-v7a
message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message (Build Type: $$DEFINES)
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } android-g++{
        target.path =  $$[QT_INSTALL_PREFIX]/lib
    }
    else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/values-rs/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/drawable-ldpi/icon.png \
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
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/values-nb/strings.xml \
    android/res/values-es/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-id/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-et/strings.xml \
    android/version.xml \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl

android-g++{
LIBS += -L$$PWD/../../../../platforms/Android/androidPlugins/armeabi-v7a/ -lMediaHandlers
INCLUDEPATH += $$PWD/../MediaHandlers
DEPENDPATH += $$PWD/../../../platforms/Android/androidPlugins/armeabi-v7a/
}
