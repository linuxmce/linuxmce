#!/bin/bash
export ANDROID_NDK_PLATFORM=android-9
export ANDROID_NDK_ROOT=/home/langston/necessitas4/android-ndk
export ANDROID_SDK_ROOT=/home/langston/necessitas4/android-sdk
export SDK_BASE=/home/langston/necessitas4/android-sdk
export ANDROID_NDK_TOOLCHAIN_VERSION=4.6
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/games:/usr/games:/home/langston/necessitas4/android-ndk:/home/langston/necessitas4/android-ndk/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86/bin:/home/langston/necessitas4/android-sdk/tools:/home/langston/necessitas4/android-sdk/platform-tools

cd qOrbiter/qOrbiter_src/qt5droid/Qt5Android
rm -rf android-build
mkdir android-build
/home/langston/Qt5.2.0-android/5.2.0-beta1/android_armv7/bin/qmake Qt5Android.pro -r -spec android-g++ CONFIG+=debug
make -j3
make install INSTALL_ROOT=android-build
cp /home/langston/Qt5.2.0-android/5.2.0-beta1/android_armv7/bin/androiddeployqt .
chmod u+x androiddeployqt
./androiddeployqt --input android-libqorbiterqt5.so-deployment-settings.json --output android-build/

mv android-build/bin/QtApp-debug.apk android-build/bin/linuxmce-qorbiter-qt5-debug.apk
exit 0