#!/bin/bash
clear
#Work Area
CURRENT_DIRECTORY=""
CURRENT_SKIN_PATH=""
QML_ROOT=""
target_paths=( "android" "desktop" "rpi" "qt5-desktop" )

function processSkinDirectory(){

echo "Processing $CURRENT_SKIN_PATH" 
cd $CURRENT_SKIN_PATH
#ls -lha
echo "
"
cd $QML_ROOT

}

function lookupDir(){

echo "Searching::$CURRENT_DIRECTORY"
#ls -lha $CURRENT_DIRECTORY

#now we go through and find skin directories

for currentDir in $CURRENT_DIRECTORY/*
do

	if [ -d "$currentDir" ]; then
	echo "$currentDir is to be processed"
	CURRENT_SKIN_PATH="$currentDir"
	processSkinDirectory
	else
	echo "$currentDir is a file and will be skipped"
	fi

done

echo "Done with $CURRENT_DIRECTORY"

}

#----------
clear
echo "LinuxMCE QML manifest export - Automated qmldir creator"
echo "Switching to qml root"
cd ../../
QML_ROOT=$(pwd)
echo $QML_ROOT" is qml root dir"
echo "Beginning"
echo "Target Paths::" ${target_paths[@]}

for dir in "${target_paths[@]}"
do
TEST_PATH=$(pwd)"/""$dir"
echo $TEST_PATH
CURRENT_DIRECTORY=$TEST_PATH
lookupDir
done





