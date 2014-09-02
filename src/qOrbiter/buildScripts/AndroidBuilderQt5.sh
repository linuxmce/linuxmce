#!/bin/bash
#This script will build the Qt for Android (Qt5) variant of QOrbiter assuming the prerequsites are in place.
#Below are the environtal variables which you should fill in to adjust for your local machine.
START=$(pwd)
ANDROID_INSTALL=/home/langston/Qt5.2.0-android/5.2.0-beta1/android_armv7/bin
ANDROIDNDKPLATFORM=android-9
ANDROIDNDKROOT=/home/langston/necessitas4/android-ndk
ANDROIDSDKROOT=/home/langston/necessitas4/android-sdk
SDKBASE=/home/langston/necessitas4/android-sdk
TOOLCHAINVERSION=4.6
PATHADDS=:/home/langston/necessitas4/android-ndk:/home/langston/necessitas4/android-ndk/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86/bin:/home/langston/necessitas4/android-sdk/tools:/home/langston/necessitas4/android-sdk/platform-tools

export ANDROID_NDK_PLATFORM=$ANDROIDNDKPLATFORM
export ANDROID_NDK_ROOT=$ANDROIDNDKROOT
export ANDROID_SDK_ROOT=$ANDROIDSDKROOT
export SDK_BASE=$ANDROIDSDKBASE
export ANDROID_NDK_TOOLCHAIN_VERSION=$TOOLCHAINVERSION
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/games:/usr/games$PATHADDS

clear
cd ../qOrbiter_src/plugins/AudioVisual
make clean 
$ANDROID_INSTALL/qmake AudioVisual.pro -r -spec android-g++ "CONFIG+=opengl"
make clean -j6


clear
cd $START
cd ../qOrbiter_src/plugins/DceScreenSaver
make clean 
$ANDROID_INSTALL/qmake DceScreenSaver.pro -r -spec android-g++ "CONFIG+=opengl"
make clean -j6


cd $START
cd ../qOrbiter_src/qt5droid/Qt5Android
rm -rf android-build
mkdir android-build

$ANDROID_INSTALL/qmake Qt5Android.pro -r -spec android-g++ CONFIG+=debug
make clean -j6
make install INSTALL_ROOT=android-build
cp $ANDROID_INSTALL/androiddeployqt .
chmod u+x androiddeployqt
./androiddeployqt --input android-libqorbiterqt5.so-deployment-settings.json --output android-build/
mv android-build/bin/QtApp-debug.apk android-build/bin/linuxmce-qorbiter-qt5-debug.apk
clear
ls -lha android-build/bin/
exit 0;