#!/bin/bash
. /usr/pluto/bin/Utils.sh

## Remove mediatomb web bookmark
DelBookmark "http://dcerouter:49152/"

## Remove the cron entry
#cat  /etc/crontab  | grep -v "StorageDevices_Radar.sh" > /etc/crontab.$$
#mv /etc/crontab.$$ /etc/crontab
rm -f /etc/cron.d/StorageDevicesRadar

## Remove the init script that does the samba server scanning
rm -f /etc/rc2.d/StorageDevices_SambaRadar.sh
rm -f /etc/init.d/StorageDevices_SambaRadar.sh

## Remove the init script that does the status (online/offline) scanning
rm -f /etc/rc2.d/StorageDevices_StatusRadar.sh
rm -f /etc/init.d/StorageDevices_StatusRadar.sh

## Remove umount diversion
if [ remove = "$1" ]; then 
	dpkg-divert --package pluto-storage-devices --remove --rename --divert /bin/umount.wraped /bin/umount
	dpkg-divert --package pluto-storage-devices --remove --rename --divert /bin/mount.wraped /bin/umount
fi
