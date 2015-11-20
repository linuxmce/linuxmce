#!/bin/bash
# Pluto System Restore
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

# vars
MASTERDIR="/home/backup"
BKPDIR=$(date +%Y-%m-%d-%H-%M)
FULLPATH="$MASTERDIR/download/$BKPDIR"
DATABASEFOLDER="database"
FILESYSTEMFOLDER="filesystem"
VERSION=$(awk -F= '$1=="DISTRIB_RELEASE"{ sub(/\./,""); printf "%04d\n", $2 }' /etc/lsb-release)

#Ensure the backup and upload directory exists
if [ ! -d /home/backup/upload ]; then
	mkdir -p /home/backup/upload
fi

Usage () {
	echo "$0 [command]"
	echo "Commands :"
	echo "	--backup -> create a system backup"
	echo "  --autobackup -> create a small backup of the system database"
	echo "	--restore -> restore system from a backup file"
	echo "	--restore --skip-md5 -> system restore without md5 control file"
	exit
}

# RESTORE
# This function will restore backup archives made with LinuxMCE 0810 and later.
# Starting with 0810, a new "Version" file will be created in the root of the backup archive.
# Since 0710 and earlier didn't contain a "Version" file, it creates an easy way to determine
# whether the archive was made prior to version 0810. It also gives us a basis on which
# we can add logic to deal with upgrading from specific versions in the future.
Restore() {
	BKPDIR=$(find $MASTERDIR/upload -maxdepth 1 -mindepth 1 -type d)

	# ----------------------
	# Restore Database Dumps
	# ----------------------
	cd $BKPDIR/$DATABASEFOLDER
	# Get a list of all the databases that were backed up
	Databases=$(ls -d */ | sed '1,$s/\///g') # sed removes trailing "/" character
	for thisDatabase in $Databases
	do
		# Get a list of all the backup files to restore to this database
		BackupFiles=$(ls $thisDatabase)
		for thisFile in $BackupFiles
		do
			/usr/bin/mysql -u root -D $thisDatabase < $thisDatabase/$thisFile
		done
	done

	# -------------
	# Restore Files
	# -------------
	cp -ar $BKPDIR/$FILESYSTEMFOLDER/* /
	# Boy, that was simple!

	# -------------
	# MISC. CLEANUP
	# -------------
	chown -R  www-data /usr/pluto/orbiter/floorplans/
	service asterisk restart
	
	## Reinstall this packages so we'll have an updates database schema
	apt-get -y --reinstall install pluto-system-database
	apt-get -y --reinstall install pluto-media-database
	apt-get -y --reinstall install pluto-local-database
	apt-get -y --reinstall install pluto-security-database
	apt-get -y --reinstall install pluto-telecom-database
	
	if [[ -f /var/lib/dpkg/info/freepbx.postinst ]] ;then
		apt-get -y --reinstall install freepbx
	fi
	
	## Mark MDs for reconfigure
	RunSQL "UPDATE Device SET NeedConfigure = '1' WHERE FK_DeviceTemplate = 28"

	# From 0704 to 0710
	ConfDel remote

	# Update Version/Architecture infromation
	arch=$(apt-config dump | grep 'APT::Architecture' | sed 's/APT::Architecture.*"\(.*\)".*/\1/g')
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='LMCE_CORE_u${VERSION}_${arch}' WHERE IK_DeviceData='LMCE_CORE_1_1'"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='LMCE_MD_u${VERSION}_i386'   WHERE IK_DeviceData='LMCE_MD_1_1'"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='0' WHERE FK_DeviceData='234'"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='i386' WHERE FK_DeviceData='112' AND IK_DeviceData='686'"
	RunSQL "USE asterisk; UPDATE incoming SET destination=CONCAT('custom-linuxmce', SUBSTR(destination, 18)) WHERE destination LIKE 'from-pluto-custom%'"
	RunSQL "DELETE FROM Software_Device WHERE 1"

	# Delete Disk Drives and let them redetect
	DT_DiskDrive=11
	DiskDrives=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DT_DiskDrive'")
	for DiskDrive in $DiskDrives ;do
		DiskDrive_DeviceID=$(Field 1 "$DiskDrive")
		for table in 'CommandGroup_Command' 'Device_Command' 'Device_CommandGroup' 'Device_DeviceData' 'Device_DeviceGroup' 'Device_Device_Related' 'Device_EntertainArea' 'Device_HouseMode' 'Device_Orbiter' 'Device_StartupScript' 'Device_Users' ;do
			RunSQL "DELETE FROM \`$table\` WHERE FK_DeviceID = '$DiskDrive_DeviceID' LIMIT 1"
		done
		RunSQL "DELETE FROM Device WHERE PK_Device = '$DiskDrive_DeviceID' LIMIT 1"
	done

	# Clean up after ourselves
	rm -rf $MASTERDIR/upload/*
	rmdir $MASTERDIR/upload

	# TODO: Is this needed, since pluto.conf was restored??
	ConfSet "Display" "$Display"
	ConfSet "AutostartCore" "$AutostartCore"
	ConfSet "AutostartMedia" "$AutostartMedia"

	/usr/pluto/bin/Network_Setup.sh
	/usr/pluto/bin/Diskless_Setup.sh
}


# LEGACY RESTORE
# The backup archive folder structure was different in versions
# prior to 0810. A move was made in version 0810 to clean up the backup/restore
# code so that restores can be done programatically and not need special cases
# for each database/table and filesystem file that needs restored.
# This legacy function will restore from backups made with 0710 and earlier.
# TODO: This support can probably be removed eventually
LegacyRestore() {
	BKPDIR=$(find $MASTERDIR/upload -maxdepth 1 -mindepth 1 -type d)
	# process data from backup
	cp -L -r $BKPDIR/etc/* /etc
	cp -L -r $BKPDIR/usr/* /usr
	chown -R  www-data /usr/pluto/orbiter/floorplans/
	# restore mysql
	cd $BKPDIR/mysql
	dbtables=$(ls)
	# restore old mysql data
	for table in $dbtables
	do
		/usr/bin/mysql -u root -D pluto_main < $table
	done
	               
	# restore asterisk settings
	cd $BKPDIR/asterisk
	/usr/bin/mysql -u root asterisk < asterisk.sql
	service asterisk restart

	## Reinstall this packages so we'll have an updates database schema
	apt-get -y --reinstall install pluto-system-database
	apt-get -y --reinstall install pluto-media-database
	apt-get -y --reinstall install pluto-local-database
	apt-get -y --reinstall install pluto-security-database
	apt-get -y --reinstall install pluto-telecom-database
	if [[ -f /var/lib/dpkg/info/freepbx.postinst ]] ;then
		apt-get -y --reinstall install freepbx
	fi

	## Mark MDs for reconfigure
	RunSQL "UPDATE Device SET NeedConfigure = '1' WHERE FK_DeviceTemplate = 28"

	# From 0704 to 0710
	ConfDel remote

	arch=$(apt-config dump | grep 'APT::Architecture' | sed 's/APT::Architecture.*"\(.*\)".*/\1/g')
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='LMCE_CORE_u${VERSION}_${arch}' WHERE IK_DeviceData='LMCE_CORE_1_1'"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='LMCE_MD_u${VERSION}_i386'   WHERE IK_DeviceData='LMCE_MD_1_1'"
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

}

if [[ -z $1 ]]; then
	Usage
fi

# create restore point
if [[ "$1" == "--backup" ]] || [[ "$1" == "--autobackup" ]]; then 
	BackupDescription="$2"
	if [[ "$1" == "--backup" ]]; then
		BACKUPDESTINATION="backup-$BKPDIR"
	else
		BACKUPDESTINATION="backup-$BKPDIR-auto"
	fi
	# Make the main backup directory if it doesn't exist
	if [[ ! -d $MASTERDIR ]]; then
		mkdir -p $MASTERDIR/download
	fi
	# Make the directory for this specific backup if it doesn't exist
	if [[ ! -d $BKPDIR ]]; then
		mkdir -p $MASTERDIR/download/$BKPDIR
	fi
	mkdir -p "$FULLPATH"

	# Make the README File
	echo "$BackupDescription" > "$FULLPATH/README"

	# Make the Version file
	echo "$VERSION" > "$FULLPATH/Version"
	
	# ----------------------
	# BACKUP DATABASE TABLES
	# ----------------------
	mkdir -p $FULLPATH/$DATABASEFOLDER

	# Backup pluto_main tables
	mkdir -p $FULLPATH/$DATABASEFOLDER/pluto_main
	dbtables=$(RunSQL "select Tablename from psc_local_tables")
	dbtables="$dbtables psc_local_batdet psc_local_bathdr psc_local_batuser psc_local_repset psc_local_schema psc_local_tables"
	for table in $dbtables
	do
		/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table --skip-extended-insert $MYSQL_DB_CRED "$MySqlDBName" $table  > $FULLPATH/$DATABASEFOLDER/pluto_main/$table.sql
	done

	# Backup asterisk database
	mkdir -p $FULLPATH/$DATABASEFOLDER/asterisk
	/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table --skip-extended-insert $MYSQL_DB_CRED asterisk > $FULLPATH/$DATABASEFOLDER/asterisk/asterisk.sql

	# Backup mythtv database
	mkdir -p $FULLPATH/$DATABASEFOLDER/mythconverg
	/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table --skip-extended-insert $MYSQL_DB_CRED mythconverg > $FULLPATH/$DATABASEFOLDER/mythconverg/mythconverg.sql

	if [[ "$1" == "--backup" ]] ; then	
		# Backup pluto_media database
		mkdir -p $FULLPATH/$DATABASEFOLDER/pluto_media
		/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table --skip-extended-insert $MYSQL_DB_CRED pluto_media > $FULLPATH/$DATABASEFOLDER/pluto_media/pluto_media.sql
	
		# -----------------------
		# BACKUP FILESYSTEM FILES
		# -----------------------
		mkdir -p $FULLPATH/$FILESYSTEMFOLDER
	
		# Backup lmce-admin
		mkdir -p $FULLPATH/$FILESYSTEMFOLDER/usr/pluto/orbiter
		cp -Lr /usr/pluto/orbiter/floorplans $FULLPATH/$FILESYSTEMFOLDER/usr/pluto/orbiter
		cp -Lr /usr/pluto/orbiter/users $FULLPATH/$FILESYSTEMFOLDER/usr/pluto/orbiter
		cp -Lr /usr/pluto/orbiter/rooms $FULLPATH/$FILESYSTEMFOLDER/usr/pluto/orbiter
		cp -Lr /usr/pluto/orbiter/scenarios $FULLPATH/$FILESYSTEMFOLDER/usr/pluto/orbiter
	
		# Backup coverart and attribute
		mkdir -p $FULLPATH/$FILESYSTEMFOLDER/home/mediapics
		cp -Lr /home/mediapics $FULLPATH/$FILESYSTEMFOLDER/home	
		
		# Backup conf files
		mkdir -p $FULLPATH/$FILESYSTEMFOLDER/etc
		cp -r /etc/pluto.conf $FULLPATH/$FILESYSTEMFOLDER/etc

		# Backup Camera Archives
		mkdir -p $FULLPATH/$FILESYSTEMFOLDER/home/cameras
		cp -Lr /home/cameras/ $FULLPATH/$FILESYSTEMFOLDER/home

		# Backup game player data if available
		if [[ -d /home/mamedata ]]; then
			mkdir -p $FULLPATH/$FILESYSTEMFOLDER/home/mamedata/cfg
			cp -Lr /home/mamedata/cfg/ $FULLPATH/$FILESYSTEMFOLDER/home/mamedata
			mkdir -p $FULLPATH/$FILESYSTEMFOLDER/home/mamedata/inp
			cp -Lr /home/mamedata/inp/ $FULLPATH/$FILESYSTEMFOLDER/home/mamedata
			mkdir -p $FULLPATH/$FILESYSTEMFOLDER/home/mamedata/memcard
			cp -Lr /home/mamedata/memcard/ $FULLPATH/$FILESYSTEMFOLDER/home/mamedata
			mkdir -p $FULLPATH/$FILESYSTEMFOLDER/home/mamedata/nvram
			cp -Lr /home/mamedata/nvram/ $FULLPATH/$FILESYSTEMFOLDER/home/mamedata
			mkdir -p $FULLPATH/$FILESYSTEMFOLDER/home/mamedata/sta
			cp -Lr /home/mamedata/sta/ $FULLPATH/$FILESYSTEMFOLDER/home/mamedata
		fi

	fi
	# ----------------
	# Make the archive
	# ----------------
	cd $MASTERDIR/download	
	# TODO:  make cli option to use either Gzip or Bzip compression (and make an option in the web admin), as the restore section can handle either...
	tar cfz $BACKUPDESTINATION.tar.gz $BKPDIR
	md5sum $BACKUPDESTINATION.tar.gz > $BACKUPDESTINATION.md5

	echo "Restore point created."
	rm -rf $FULLPATH
fi  

# restore system from backup
if [[ "$1" == "--restore" ]]; then
	cd $MASTERDIR/upload
	backupfile=$(ls $MASTERDIR/upload | grep tar)
	
	if [[ "$backupfile" != "" ]]; then
		if [[ -n "$USE_WHIPTAIL" ]]; then
			chvt 8
			whiptail --infobox "Restoring database from backup" 10 0 >/dev/tty8
		fi
		
		# ------------------------
		# UNZIP THE BACKUP ARCHIVE
		# ------------------------
		# Find the compression type and unzip with the correct options
		echo "Decompressing $backupfile"
		if [[ ${backupfile##*.} == "gz" ]]; then
			tar xf "$backupfile"
		elif [[ ${backupfile##*.} == "bz2" ]]; then
			tar xfj "$backupfile"
		else
			echo "File extension ${backupfile##*.} not supported! Exiting!"
			exit 1
		fi

		# Find the extracted folder
                BKPDIR=$(find $MASTERDIR/upload -maxdepth 1 -mindepth 1 -type d)

		# ----------------------
		# RESTORE BACKUP ARCHIVE
		# ----------------------
		# Determine whether or not this is a new backup archive, or an old one - then use the appropriate restore method
		if [[ -a $BKPDIR/Version ]]; then
			echo "Version File Found, using New Restore method"
			#call new restore method
			Restore
		else
			echo "Version File not found. Using Legacy Restore method"
			#call legacy restore method
			LegacyRestore
		fi

		if [[ -n "$USE_WHIPTAIL" ]]; then
			clear >/dev/tty8
			chvt 1
		fi
	fi
fi



