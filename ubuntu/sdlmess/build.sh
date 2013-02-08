#!/bin/bash

ARCH=`uname -m`


. /etc/lmce-build/builder.conf

MKOPTS="PTR64=0"
# If we build for amd64 we need to set an env for mame
if [ $arch = "amd64"  ]
then
	echo Adding 64-bit pointer handling.
	MKOPTS="PTR64=1"	
fi

make NOWERROR=1 TARGET=mess $MKOPTS all

