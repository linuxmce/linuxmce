#!/bin/sh /etc/rc.common

START=03
log_file="/tmp/log.upgrade"

start() {
	BackupID=`nvram get backupid`
	if [[ "$BackupID" != "" ]] ;then
		
		# Restore critical config files
		echo "`date` - Restore critical config files" >> $log_file
		nvram get backup | uudecode > /tmp/bk2nvram.tar.gz
		cd /
       		echo "`date` - Upacking files..." >>$log_file
		tar zxvf /tmp/bk2nvram.tar.gz 1>>$log_file 2>&1
		#rm -f /tmp/bk2nvram.tar.gz #it's deleted in freshinstall
	    
        echo "`date` - Clearing nvram backup variable" >> $log_file
		nvram set backup=""
		#nvram commit
	fi
}

stop() {
	:
}

