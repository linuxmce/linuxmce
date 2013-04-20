#!/bin/bash

ARCH=`uname -m`

. /etc/lmce-build/builder.conf

make_jobs=""
[[ 1 -lt "$NUM_CORES" ]]  && make_jobs="-j $NUM_CORES"

MKOPTS="PTR64=0"
# If we build for amd64 we need to set an env for mame
if [ $arch = "amd64"  ]
then
	echo Adding 64-bit pointer handling.
	MKOPTS="PTR64=1"
fi

make $make_jobs NOWERROR=1 $MKOPTS all

