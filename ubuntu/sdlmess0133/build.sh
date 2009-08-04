#!/bin/bash

ARCH=`uname -m`

if [ $ARCH = "x86_64"  ]
then
	echo Adding 64-bit pointer handling.
	MKOPTS="PTR64=1"	
fi

make $MKOPTS -f makefile.sdl

