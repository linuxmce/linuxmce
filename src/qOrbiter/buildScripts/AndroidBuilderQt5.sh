#!/bin/bash
#This script will build the Qt for Android (Qt5) variant of QOrbiter assuming the prerequsites are in place.
#Below are the environment variables which you should fill in to adjust for your local machine.
#
#ANDROID_INSTALL should point to qmake for Android. See current example.
#ANDROIDNDKPLATFORM is for use by the build system and must be set. no need to change.
#ANTPATH=if you are not using necessitas or this var is empty, it will be set here.
#JAVAHOME points to the java library needed to compile java classes.
#
#
#
INSTALLTODEVICE=""
CLEANBUILD=""
PCOUNT=1

OPTIND=1 
        while getopts "i:c:j:" opt; do 
        case "$opt" in 
                i)  INSTALLTODEVICE="Y"
        ;; 
                c)  CLEANBUILD="Y" 
        ;;
         	j)  PCOUNT=$OPTARG 
        ;; 
        esac 
        done 
        

START=$(pwd)
ANDROID_INSTALL=/home/langston/Qt5.3.2/5.3/android_armv7/bin
ANDROID_SDK_PATH=/home/langston/necessitas
ANDROIDNDKPLATFORM=android-9
ANDROIDNDKROOT=$ANDROID_SDK_PATH/android-ndk
ANDROIDSDKROOT=$ANDROID_SDK_PATH/android-sdk
TOOLCHAINVERSION=4.6
PATHADDS=:/home/langston/necessitas/android-ndk:/home/langston/necessitas/android-ndk/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86/bin:/home/langston/necessitas/android-sdk/tools:/home/langston/necessitas/android-sdk/platform-tools

BUILTPLUGINS=$START/../platforms/Android/androidPlugins/Qt5/armeabi-v7a
ANTPATH=/home/langston/necessitas/apache-ant-1.8.4/bin/ant
JAVAHOME=/usr/lib/jvm/java-6-openjdk-i386
export ANDROID_NDK_PLATFORM=$ANDROIDNDKPLATFORM
export ANDROID_NDK_ROOT=$ANDROIDNDKROOT
export ANDROID_SDK_ROOT=$ANDROIDSDKROOT
export SDK_BASE=$ANDROIDSDKBASE
export ANDROID_NDK_TOOLCHAIN_VERSION=$TOOLCHAINVERSION
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/games:/usr/games$PATHADDS
export JAVA_HOME=$JAVAHOME
echo "Removing old plugins"
rm -v ../platforms/Android/Qt5/armeabi-v7a/*.so

clear
cd ../qOrbiter_src/plugins/AudioVisual

if [ -e "moc*" ]; then
make clean 
echo "Cleaning up"
fi

$ANDROID_INSTALL/qmake AudioVisual.pro -r -spec android-g++ "CONFIG+=opengl"
make -j$PCOUNT

if [ -e $BUILTPLUGINS/libAudioVisual.so ];then
echo "Audio Visual Build Complete."
else
echo "Failed to build Av-Plugin!"
ls -lha $BUILTPLUGINS
exit 1;
fi

clear
cd $START
cd ../qOrbiter_src/plugins/DceScreenSaver
make clean
if [ -e "moc*" ]; then
make clean 
echo "Cleaning up"
fi

$ANDROID_INSTALL/qmake DceScreenSaver.pro -r -spec android-g++ "CONFIG+=opengl"
make -j$PCOUNT
if [ -e $BUILTPLUGINS/libDceScreenSaver.so ];then
echo "ScreenSaver Build Complete."
else
echo "Failed to build Screen Saver Plugin!"
exit 1;
fi



cd $START
cd ../qOrbiter_src/qt5droid/Qt5Android
rm -rf android-build
mkdir -p  android-build/bin
if [ -e "moc_*" ]; then
make clean 
echo "Cleaning up"
fi


$ANDROID_INSTALL/qmake Qt5Android.pro -r -spec android-g++ CONFIG+=debug
make -j$PCOUNT
make install INSTALL_ROOT=android-build
cp $ANDROID_INSTALL/androiddeployqt .
chmod u+x androiddeployqt
./androiddeployqt --input android-libqorbiterqt5.so-deployment-settings.json --output android-build/ --ant $ANTPATH
mv android-build/bin/QtApp-debug.apk android-build/bin/linuxmce-qorbiter-qt5-debug.apk
clear
ls -lha android-build/bin/*.apk

if [ -n "$INSTALLTODEVICE" ]; then

clear
echo "Installing APK"
$ANDROID_SDK_ROOT/platform-tools/./adb install android-build/bin/linuxmce-qorbiter-qt5-debug.apk
fi;



exit 0;
