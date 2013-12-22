#!/bin/bash

#Work Area
CURRENT_DIRECTORY=""
target_paths=( "android" "desktop" "rpi" "qt5-desktop" "skins-common" )
function lookupDir(){

echo "Searching::$CURRENT_DIRECTORY"
echo "

"
}

#----------
clear
echo "LinuxMCE QML manifest export - Automated qmldir creator"
echo "Switching to qml root"
cd ../../
ls -lha
echo "Beginning"
echo "Target Paths::" ${target_paths[@]}

for dir in "${target_paths[@]}"
do
TEST_PATH=$(pwd)"/""$dir"
echo $TEST_PATH
CURRENT_DIRECTORY=$TEST_PATH
lookupDir
done





