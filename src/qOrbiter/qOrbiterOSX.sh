#!/bin/bash
#QT Install Path
${QT_INSTALL_DIR:?"Need to set Qt Install Path"}

echo "Starting builder script for QOrbiter-OSX v1"
rm -rf build-output/qorbiter-core-gl.app 
rm build-output/*.dmg

$QT_INSTALL_DIR/bin/qmake qOrbiterMaster/qOrbiterMaster.pro
make clean
make
$QT_INSTALL_DIR/bin/./macdeployqt build-output/qorbiter-core-gl.app -dmg
exit 0;
