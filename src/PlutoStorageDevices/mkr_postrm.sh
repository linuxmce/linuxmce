#!/bin/bash
. /usr/pluto/bin/Utils.sh

## Remove umount diversion
if [ remove = "$1" ]; then 
	dpkg-divert --package pluto-storage-devices --remove --rename --divert /bin/umount.wraped /bin/umount
	dpkg-divert --package pluto-storage-devices --remove --rename --divert /bin/mount.wraped /bin/umount
fi
