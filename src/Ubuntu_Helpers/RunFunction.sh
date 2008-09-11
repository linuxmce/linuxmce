#!/bin/bash

export SVNROOT=$1
export SCRIPTLOC=$SVNROOT/src/Ubuntu_Helpers
export build_root_dir=$SVNROOT/build
export build_dir=$build_root_dir/bin
export local_mirror_dir=$build_root_dir/www
mkdir -p $build_root_dir
mkdir -p $build_dir
mkdir -p $local_mirror_dir
export build_log=$SVNROOT/build/build.log

source $SCRIPTLOC/BuildUbuntu_Functions_SelfHost.sh

$2 $3 $4 $5 $6 $7 $8 $9

