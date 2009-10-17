#!/bin/bash
# Pluto System Restore

# vars
MASTERDIR="/home/backup"
BKPDIR=$(date +%Y-%m-%d-%H-%M)
FULLPATH="$MASTERDIR/download/$BKPDIR"

Usage () {
	echo "$0 [command]"
	echo "Commands :"
	echo "	--backup -> create a system backup"
	echo "	--restore -> restore system from a backup file"
	echo "	--restore --skip-md5 -> system restore without md5 control file"
	exit
}


if [[ -z $1 ]]; then
	Usage
fi

# create restore point
if [[ "$1" == "--backup" ]]; then 
	BackupDescription="$2"

	#rm -rf $MASTERDIR
	if [[ ! -d $MASTERDIR ]]; then
		mkdir -p $MASTERDIR/download
	fi
	
	if [[ ! -d $BKPDIR ]]; then
		mkdir -p $MASTERDIR/download/$BKPDIR
	fi
	
	mkdir -p "$FULLPATH"
	echo "$BackupDescription" > "$FULLPATH/README"
	
	# backup database tables 
	mkdir -p $FULLPATH/mysql
	dbtables=$(RunSQL "select Tablename from psc_local_tables")
	dbtables="$dbtables psc_local_batdet psc_local_bathdr psc_local_batuser psc_local_repset psc_local_schema psc_local_tables"
	
	for table in $dbtables
	do
		/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table --skip-extended-insert $MYSQL_DB_CRED "$MySqlDBName" $table  > $FULLPATH/mysql/$table-$BKPDIR.sql
	done

	# backup asterisk database
	mkdir -p $FULLPATH/asterisk
	/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table --skip-extended-insert $MYSQL_DB_CRED asterisk > $FULLPATH/asterisk/asterisk.sql

	# backup mythtv database
	mkdir -p $FULLPATH/mythtv
	/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table --skip-extended-insert $MYSQL_DB_CRED mythtvconverg > $FULLPATH/mythtv/mythconverg.sql
	
	# backup lmce-admin
	mkdir -p $FULLPATH/usr/pluto/orbiter
	cp -Lr /usr/pluto/orbiter/floorplans $FULLPATH/usr/pluto/orbiter
	cp -Lr /usr/pluto/orbiter/users $FULLPATH/usr/pluto/orbiter
	cp -Lr /usr/pluto/orbiter/rooms $FULLPATH/usr/pluto/orbiter
	cp -Lr /usr/pluto/orbiter/scenarios $FULLPATH/usr/pluto/orbiter

	# backup coverart and attribute
	mkdir -p $FULLPATH/home/mediapics
	cp -Lr /home/mediapics $FULLPATH/home/mediapics	
	mkdir -p $FULLPATH/pluto_media
	/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table --skip-extended-insert $MYSQL_DB_CRED pluto_media > $FULLPATH/pluto_media/pluto_media.sql
	
	# backup conf files
	mkdir -p $FULLPATH/etc
	cp -r /etc/pluto.conf $FULLPATH/etc
	
	# make archive
	cd $MASTERDIR/download	
	tar cfz backup-$BKPDIR.tar.gz $BKPDIR
	md5sum backup-$BKPDIR.tar.gz > backup-$BKPDIR.md5

	echo "Restore point created."
	rm -rf $FULLPATH
fi  

# restore system from backup
if [[ "$1" == "--restore" ]]; then
	. /usr/pluto/bin/SQL_Ops.sh
	. /usr/pluto/bin/Config_Ops.sh

	cd $MASTERDIR/upload
	backupfile=$(ls $MASTERDIR/upload | grep tar)
	
	if [[ "$backupfile" != "" ]]; then
		if [[ -n "$USE_WHIPTAIL" ]]; then
			chvt 8
			whiptail --infobox "Restoring database from backup" 10 0 >/dev/tty8
		fi
		. /usr/pluto/bin/Config_Ops.sh

		tar xf "$backupfile"
		BKPDIR=$(find $MASTERDIR/upload -maxdepth 1 -mindepth 1 -type d)
		# process data from backup
		cp -L -r $BKPDIR/etc/* /etc
		cp -L -r $BKPDIR/usr/* /usr
		chown -R  www-data /usr/pluto/orbiter/floorplans/
		# restore mysql
		cd $BKPDIR/mysql
		dbtables=$(ls)
		# restore old database data
		for table in $dbtables
		do
			RunSQL < $table
		done
		
		# restore asterisk settings
		cd $BKPDIR/asterisk
		UseDB asterisk
		RunSQL < asterisk.sql
		UseDB "$MySqlDBName"
		/etc/init.d/asterisk restart

		# restore mythtv settings
		cd $BKPDIR/mythtv
		UseDB mythtvconverg
		RunSQL < mythtvconverg.sql
		UseDB "$MySqlDBName"
		/etc/init.d/mythtv restart

		## Reinstall this packages so we'll have an updates database schema
		apt-get -y --reinstall install pluto-system-database
		apt-get -y --reinstall install pluto-media-database
		apt-get -y --reinstall install pluto-local-database
		apt-get -y --reinstall install pluto-security-database
		apt-get -y --reinstall install pluto-telecom-database
		if [[ -f /var/lib/dpkg/info/freepbx.postinst ]] ;then
			apt-get -y --reinstall install freepbx
		fi
		
		# Restore the media file details.
		#
		# TODO: amend the directory names to point to the new location of media files.
		mysql $MYSQL_DB_CRED -f pluto_media < $BKPDIR/pluto_media/pluto_media.sql
		
		# restore old mediapics
		cp -Lr $BKPDIR/home/mediapics/* /home/mediapics
	
		## Mark MDs for reconfigure
		RunSQL "UPDATE Device SET NeedConfigure = '1' WHERE FK_DeviceTemplate = 28"

		# From 0704 to 0710
		ConfDel remote

		arch=$(apt-config dump | grep 'APT::Architecture' | sed 's/APT::Architecture.*"\(.*\)".*/\1/g')
		RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='LMCE_CORE_u0810_$arch' WHERE IK_DeviceData='LMCE_CORE_1_1'"
		RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='LMCE_MD_u0810_i386'   WHERE IK_DeviceData='LMCE_MD_1_1'"
		RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='0' WHERE FK_DeviceData='234'"
		RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='i386' WHERE FK_DeviceData='112' AND IK_DeviceData='686'"
		RunSQL "USE asterisk; UPDATE incoming SET destination=CONCAT('custom-linuxmce', SUBSTR(destination, 18)) WHERE destination LIKE 'from-pluto-custom%'"
		RunSQL "DELETE FROM Software_Device WHERE 1"

		DT_DiskDrive=11
		DiskDrives=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DT_DiskDrive'")
		for DiskDrive in $DiskDrives ;do
			DiskDrive_DeviceID=$(Field 1 "$DiskDrive")
			for table in 'CommandGroup_Command' 'Device_Command' 'Device_CommandGroup' 'Device_DeviceData' 'Device_DeviceGroup' 'Device_Device_Related' 'Device_EntertainArea' 'Device_HouseMode' 'Device_Orbiter' 'Device_StartupScript' 'Device_Users' ;do
				RunSQL "DELETE FROM \`$table\` WHERE FK_DeviceID = '$DiskDrive_DeviceID' LIMIT 1"
			done
			RunSQL "DELETE FROM Device WHERE PK_Device = '$DiskDrive_DeviceID' LIMIT 1"
		done

		rm -rf $MASTERDIR/upload/*
		rmdir $MASTERDIR/upload

		ConfSet "Display" "$Display"
		ConfSet "AutostartCore" "$AutostartCore"
		ConfSet "AutostartMedia" "$AutostartMedia"

		/usr/pluto/bin/Network_Setup.sh
		/usr/pluto/bin/Diskless_Setup.sh

		if [[ -n "$USE_WHIPTAIL" ]]; then
			clear >/dev/tty8
			chvt 1
		fi
	fi
fi
