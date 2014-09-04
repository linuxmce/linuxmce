#!/bin/bash
#This script builds qOrbiter for OS X. 
#You must install the Mac Developer tools PRIOR to installing Qt5
#You must set the path of your Qt Directory

QTINSTALL=/Users/Langstonius/Qt-5.3.0/5.3/clang_64

clear
echo "Starting builder script for QOrbiter-OSX v1"
cd ..
START=$(pwd)

rm -rf build-output/qorbiter-core-gl.app 
rm build-output/*.dmg
rm qOrbiter_src/imports/AudioVisual/*.so
rm qOrbiter_src/imports/DceScreenSaver/*.so

cd qOrbiterMaster
$QTINSTALL/bin/qmake qOrbiterMaster.pro 
#make clean
make
cd ../build-output
cp -r -v $QTINSTALL/qml/ qorbiter-core-gl.app/Contents/MacOS/imports
$QTINSTALL/bin/./macdeployqt qorbiter-core-gl.app -dmg
echo "build complete, check output."
ls -lha
exit 0;
