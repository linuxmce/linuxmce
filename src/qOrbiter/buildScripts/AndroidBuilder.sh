#!/bin/bash
#!/bin/bash
#This script will build the Qt for Android (Qt5) variant of QOrbiter assuming the prerequsites are in place.
#Below are the environment variables which you should fill in to adjust for your local machine.
#
#ANDROID_INSTALL should point to qmake for Android. See current example.
#ANDROIDNDKPLATFORM is for use by the build system and must be set. no need to change.
#ANTPATH=if you are not using necessitas or this var is empty, it will be set here.
#JAVAHOME points to the java library needed to compile java classes.
NECESSITAS_ROOT=/home/langston/necessitas
ANDROID_SDK_PATH=/home/langston/necessitas
ANDROIDNDKPLATFORM=android-9
ANDROIDNDKROOT=$ANDROID_SDK_PATH/android-ndk
ANDROIDSDKROOT=$ANDROID_SDK_PATH/android-sdk
TOOLCHAINVERSION=4.6
TOOLCHAINPATH=$ANDROIDNDKROOT/toolchains/arm-linux-androideabi-$TOOLCHAINVERSION/prebuilt/linux-x86/bin
JAVAHOME=/usr/lib/jvm/java-6-openjdk-i386
ANTPATH=$NECESSITAS_ROOT/apache-ant-1.8.4/bin/ant
# We stop whenever an error happens.
set -e

# INSTALLTODEVICE command line flag -i will install to the first found devices via adb.
# CLEANBUILD command line flag -c will clean before building.
# PCOUNT command line arg for processor count
# the local installation.
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

export JAVA_HOME=$JAVAHOME
export ANDROID_NDK_PLATFORM=android-9
export ANDROID_NDK_ROOT=$NECESSITAS_ROOT/android-ndk
export ANDROID_SDK_ROOT=$NECESSITAS_ROOT/android-sdk
export ANDROID_HOME=$ANDROIDSDKROOT
OLDPATH=$PATH
NEWPATH=$OLDPATH:$ANDROID_NDK_ROOT:$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
NEWPATH=$NEWPATH:$ANDROID_SDK_ROOT/tools:$ANDROID_SDK_ROOT/platform-tools
export PATH=$NEWPATH

TARGET=debug

# We reset the path at the end of the script.
trap 'export PATH=$OLDPATH' EXIT
BUILTPLUGINS=$START/../platforms/Android/androidPlugins/armeabi-v7a
echo "Removing old plugins"


clear	
cd ../qOrbiter_src/plugins/AudioVisual
 
 if [ "$CLEANBUILD" == "Y" ]; then
echo "Cleaning"
make clean -j$PCOUNT
fi
$NECESSITAS_ROOT/Android/Qt/482/armeabi-v7a/bin/qmake AudioVisual.pro -r -spec android-g++ "CONFIG+=opengl"
make -j$PCOUNT
clear

cd $START
cd ../qOrbiter_src/plugins/DceScreenSaver 
 if [ "$CLEANBUILD" == "Y" ]; then
echo "Cleaning"
make clean -j$PCOUNT
fi
$NECESSITAS_ROOT/Android/Qt/482/armeabi-v7a/bin/qmake DceScreenSaver.pro -r -spec android-g++ "CONFIG+=opengl"
make -j$PCOUNT

clear
cd $START

cd ../qOrbiter_src/necessitas/Qorbiter-necessitas
rm -rf installdir
mkdir installdir
 
  if [ "$CLEANBUILD" == "Y" ]; then
echo "Cleaning"
make clean -j$PCOUNT
fi
$NECESSITAS_ROOT/Android/Qt/482/armeabi-v7a/bin/qmake Qorbiter-necessitas.pro -r -spec android-g++ "CONFIG+=opengl"
make  -j$PCOUNT
make INSTALL_ROOT="android" install

cd android
$ANDROID_SDK_ROOT/tools/android update project --path .
$NECESSITAS_ROOT/apache-ant-1.8.4/bin/ant clean $TARGET

if [ -n "$INSTALLTODEVICE" ]; then

clear
echo "Installing APK"
ls -lha bin
$ANDROID_SDK_ROOT/platform-tools/./adb install bin/qOrbiterQt4-debug.apk
fi;

exit 0; 
