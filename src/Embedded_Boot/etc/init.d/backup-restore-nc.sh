#!/bin/sh /etc/rc.common

START=49
BACKUP_STORAGE_URL="http://backup.fiire.com/"
log_file="/tmp/log.upgrade"
bk2net_file="/tmp/bk2net.tar.gz"

start() {
	BackupID=`nvram get backupid`
	if [[ "$BackupID" != "" ]] ;then
		
		# Restore non critical config files
		echo "`date` - Restore non critical config files" >>$log_file
		InstallationID=`cat /etc/pluto/installation_number`
		URL="$BACKUP_STORAGE_URL/$InstallationID-$BackupID"
        echo "`date` - Dowloading $bk2net_file from $URL" >> $log_file
        #wget -O /tmp/bk2net.tar.gz "$BACKUP_STORAGE_URL/$InstallationID-$BackupID" 1>>$log_file 2>&1
       	curl -k -s -S --fail --retry 3 --connect-timeout 3 --max-time 5 -o $bk2net_file $URL 1>>$log_file 2>&1
        cd /
		echo "`date` - Upacking files..." >>$log_file
		tar zxvf /tmp/bk2net.tar.gz 1>>$log_file 2>&1
		rm -f /tmp/bk2net.tar.gz
		
		echo "`date` - Clearing Nvram variabiles." >> $log_file
		nvram set backup=""
		nvram set backupid=""
		nvram commit
	fi
}

stop() {
	:
}

