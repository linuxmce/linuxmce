#!/bin/bash
#This script builds qOrbiter for OS X. 
#You must install the Mac Developer tools PRIOR to installing Qt5
#You must set the path of your Qt Directory

QTINSTALL=/Users/Langstonius/Qt-5.3.0/5.3/clang_64

clear
echo "Starting builder script for QOrbiter-OSX v1"
cd ..
START=$(pwd)

rm -v  -rf build-output/qorbiter-core-gl.app 
rm -v build-output/*.dmg
rm -v qOrbiter_src/imports/AudioVisual/*.so
rm -v qOrbiter_src/imports/DceScreenSaver/*.so

clear
cd qOrbiterMaster
$QTINSTALL/bin/qmake qOrbiterMaster.pro  
#make clean
LOG=$(make)
cd ../build-output
if [ -e qorbiter-core-gl.app ]; then
cp -r -v $QTINSTALL/qml/ qorbiter-core-gl.app/Contents/MacOS/imports
WAIT=$($QTINSTALL/bin/./macdeployqt qorbiter-core-gl.app -dmg)
echo "build complete, check output."
ls -lha
else
echo "Failed to build app package!!"
fi
exit 0;
