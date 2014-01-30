#!/bin/bash
# We stop whenever an error happens.
set -e

# The following vars need to be modified to reflect
# the local installation.

LINUXMCE_SRC=~/lmce-trunk/src
export ANDROID_NDK_PLATFORM=android-9
NECESSITAS_ROOT=~/necessitas4
export ANDROID_NDK_ROOT=$NECESSITAS_ROOT/android-ndk
export SDK_BASE=$NECESSITAS_ROOT/android-sdk
OLDPATH=$PATH
NEWPATH=$OLDPATH:$ANDROID_NDK_ROOT:$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
NEWPATH=$NEWPATH:$ANDROID_SDK_ROOT/tools:$ANDROID_SDK_ROOT/platform-tools
export PATH=$NEWPATH


# We reset the path at the end of the script.
trap 'export PATH=$OLDPATH' EXIT
	
cd $LINUXMCE_SRC/qOrbiter/qOrbiter_src/necessitas/Qorbiter-necessitas
rm -rf installdir
mkdir installdir
make clean || :
$NECESSITAS_ROOT/Android/Qt/482/armeabi-v7a/bin/qmake Qorbiter-necessitas.pro -r -spec android-g++ "CONFIG+=opengl"
make clean -j6
make INSTALL_ROOT="android" install
