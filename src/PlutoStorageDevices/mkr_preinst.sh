#!/bin/bash

if [ install = "$1" -o upgrade = "$1" ]; then 
	dpkg-divert --add --rename --package pluto-storage-devices --divert /bin/umount.wraped /bin/umount
	dpkg-divert --add --rename --package pluto-storage-devices --divert /bin/mount.wraped /bin/mount
fi

