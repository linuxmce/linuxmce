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
srcroot=~/lmce-src/1204/src
config="CONFIG+=debug CONFIG+=for_desktop"
CLEANBUILD="Y"

# load custom build configuration if it exists
scriptDir=`dirname $0`
if [ -e "$scriptDir/build.conf" ] ; then
	. "$scriptDir/build.conf"
fi
projectdir=$srcroot/qOrbiter/qOrbiterMaster
buildoutput=$srcroot/qOrbiter/build-output
qmake="$buildenv/qmake"
if [ ! -x $qmake ] ; then
	qmake=`which qmake`
fi
echo Using $qmake
# mkdir -p $buildoutput
#/qOrbiter_src

PCOUNT=1

OPTIND=1 
        while getopts "i:c:j:" opt; do 
        case "$opt" in 
                c)  CLEANBUILD="Y" 
        ;;
         	j)  PCOUNT=$OPTARG 
        ;; 
        esac 
        done 

echo "Starting the QMake process"
pushd $buildenv
#buildoutput=$projectdir

$qmake -makefile -nocache $projectdir/qOrbiterMaster.pro -o $buildoutput/Makefile -r -spec $spec $config
if  ! [ -f $buildoutput/Makefile ] ; then
	echo "$buildoutput/Makefile does not exist - Aborting!"
	exit 1
fi

cd $buildoutput

if [ "$CLEANBUILD" == "Y" ]; then
  make clean 
  echo "Cleaning up"
fi

echo "Starting the actual build process"
make -w -j$PCOUNT
popd
