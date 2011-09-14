#!/bin/bash
#
#
#
buildenv=~/QtSDK/Desktop/Qt/4.8.0/gcc/bin
spec=linux-g++-64
srcroot=~/lmce-1004
projectdir=$srcroot/qOrbiter/qOrbiter_src
buildoutput=$srcroot/qOrbiter/build-output
mkdir -p $buildoutput
$buildenv/qmake $projectdir/qOrbiter_src.pro -r -spec $spec CONFIG+=debug CONFIG+=for_desktop
cd $buildoutput
make -w