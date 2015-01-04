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
projectdir=$srcroot/qOrbiter/qOrbiterMaster
buildoutput=$srcroot/qOrbiter/build-output
config="CONFIG+=debug CONFIG+=for_desktop"
qmake="$buildenv"+/qmake
if [ ! -x $qmake ] ; then
	qmake=`which qmake`
fi
echo Using $qmake
# mkdir -p $buildoutput
#/qOrbiter_src

echo "Starting the QMake process"
pushd $buildenv
#buildoutput=$projectdir

$qmake -makefile -nocache $projectdir/qOrbiterMaster.pro -o $buildoutput/Makefile -r -spec $spec $config
if  ! [ -f $buildoutput/Makefile ] ; then
	echo "$buildoutput/Makefile does not exist - Aborting!"
	exit 1
fi

cd $buildoutput
echo "Starting the actual build process"
make clean
make -w
popd
