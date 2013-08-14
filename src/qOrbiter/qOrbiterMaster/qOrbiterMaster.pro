TEMPLATE = subdirs

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
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

SUBDIRS =  ../LinuxMCE-Components/LinuxMCEComponents/linuxmcecomponents.pro
SUBDIRS += ../qOrbiter_src/plugins/AudioVisual
SUBDIRS += ../qOrbiter_src

#folder_01.source = ../qOrbiter_src/qml/desktop
#folder_01.target = $$DESTDIR/qml
#folder_02.source= ../qOrbiter_src/img
#folder_02.target= $$DESTDIR   #left blank so it will appear in the root
#folder_03.source = ../qOrbiter_src/qml/template
#folder_03.target = $$DESTDIR/qml
#folder_04.source = ../qOrbiter_src/config.xml
#folder_04.target = $$DESTDIR
folder_05.source = ../qOrbiter_src/import
folder_05.target = $$DESTDIR/import
DEPLOYMENTFOLDERS += folder_05
INSTALLS=$HOME
# SUBDIRS = plugins/AudioVisual

