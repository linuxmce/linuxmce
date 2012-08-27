#!/bin/bash
#
# buildenv must point to an existing QtSDK installation containing, among other
# 	things the qmake bin.
#
# spec must contain the target spec for the binary.
#
# buildoutput is the directory specified in the qOrbiter_src.pro.user file for
#	the shadow builds.
#
buildenv=/opt/pos/QtSDK.121/Desktop/Qt/4.8.1/gcc/bin
spec=linux-g++
#-64
srcroot=~/lmce-1004
projectdir=$srcroot/qOrbiter/qOrbiter_src
buildoutput=$srcroot/qOrbiter/build-output
config=CONFIG+=debug CONFIG+=for_desktop
config="CONFIG+=debug CONFIG+=for_desktop"

# mkdir -p $buildoutput
#/qOrbiter_src

echo "Starting the QMake process"
pushd $buildenv
#buildoutput=$projectdir
$buildenv/qmake -makefile -nocache $projectdir/qOrbiter_src.pro -o $buildoutput/Makefile -r -spec $spec $config
if  ! [ -f $buildoutput/Makefile ] ; then
	echo "$buildoutput/Makefile does not exist - Aborting!"
	exit 1
fi

cd $buildoutput
echo "Starting the actual build process"
make clean
make -w
popd