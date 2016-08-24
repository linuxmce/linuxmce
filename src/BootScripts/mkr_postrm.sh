#!/bin/bash

rm -rf /lib/systemd/system/linuxmce.service.d

## Remove umount diversion
if [ remove = "$1" ]; then 
	dpkg-divert --package pluto-boot-scripts --remove --rename --divert /usr/sbin/NetworkManager.wraped /usr/sbin/NetworkManager
fi
