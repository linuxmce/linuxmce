#!/bin/bash

. /etc/lmce-build/builder.conf

if [ $arch = "amd64"  ]
then
	echo Adding 64-bit pointer handling.
	MKOPTS="PTR64=1"
fi

make $MKOPTS -f makefile.sdl

