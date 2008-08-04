#!/bin/sh /etc/rc.common

START=03
BACKUP_STORAGE_URL="http://backup.fiire.com/"

start() {
	BackupID=`nvram get backupid`
	if [[ "$BackupID" != "" ]] ;then

		# Restore critical config files
		nvram get backup | uudecode > /tmp/bk2nvram.tar.gz
		cd /
		tar zxvf /tmp/bk2nvram.tar.gz
		rm -f /tmp/bk2nvram.tar.gz

		# Start networking
		/etc/init.d/network start

		# Restore non critical config files
		InstallationID=`cat /etc/pluto/installation_number`
		wget -O /tmp/bk2net.tar.gz "$BACKUP_STORAGE_URL/$InstallationID-$BackupID"
		cd /
		tar zxvf /tmp/bk2net.tar.gz
		rm -f /tmp/bk2net.tar.gz

		nvram set backup=""
		nvram set backupid=""
		nvram commit
	fi
}

stop() {
	:
}

