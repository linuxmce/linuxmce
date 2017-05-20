#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/TeeMyOutput.sh --outfile /var/log/pluto/StorageDevices_Symlinks.log --infile /dev/null --stdboth -- "$@"

TPL_GENERIC_PC_AS_CORE=7
TPL_BUFFALO_HDHG300LAN=1794
TPL_GENERIC_INTERNAL_DRIVE=1790
TPL_MAXTOR_NAS=1770
TPL_GENERIC_SAMBA_SHARE=1768
TPL_GENERIC_NFS_SHARE=1769
TPL_RAID_0=1854
TPL_RAID_1=1851
TPL_RAID_5=1849
TPL_ZFS_POOL=2351
TPL_ZFS_MIRROR=2352
TPL_ZFS_RAIDZ1=2353
TPL_ZFS_RAIDZ2=2354
TPL_ZFS_RAIDZ3=2355

DD_DIRECTORIES=153
DD_USERS=3


## Only run on Core
FK_DeviceTemplate=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device='$PK_Device'")
if [[ "$FK_DeviceTemplate" != "$TPL_GENERIC_PC_AS_CORE" ]] ;then
	exit 0
fi

## Remove old symlinks from home
echo -e "\n$(date -R) Removing old symlinks"
for userdir in /home/user_* /home/public;do
	find ${userdir}/data -xdev -mindepth 1 -maxdepth 3 -lname "*/mnt/device/*"
	find ${userdir}/data -xdev -mindepth 1 -maxdepth 3 -lname "*/mnt/device/*" -print0 | xargs -0 rm -f
done

## Maybe its best to let the user clean up the old mythtv tv_shows_* directories and symlinks...
# This will remove legacy symlinks from the tv_shows_* directories if they exist
#for userdir in /home/user_* /home/public ;do
#	find ${userdir}/data/videos -xdev -mindepth 1 -maxdepth 2 -lname "*/mnt/device/*"
#	find ${userdir}/data/videos -xdev -mindepth 1 -maxdepth 2 -lname "*/mnt/device/*" -print0 | xargs -0 rm -f
#done

## Lookup our internal storage devices in the db
Q="SELECT PK_Device, Description  FROM Device WHERE FK_DeviceTemplate IN ($TPL_GENERIC_INTERNAL_DRIVE, $TPL_GENERIC_SAMBA_SHARE, $TPL_GENERIC_NFS_SHARE, $TPL_RAID_0, $TPL_RAID_1, $TPL_RAID_5, $TPL_ZFS_POOL, $TPL_ZFS_MIRROR, $TPL_ZFS_RAIDZ1, $TPL_ZFS_RAIDZ2, $TPL_ZFS_RAIDZ3)"
InternalOwnStorageDevices=$(RunSQL "$Q")

set -o noglob
for Device in $InternalOwnStorageDevices; do
set +o noglob
	Device_ID=$(Field 1 "$Device")
	Device_Description=$(Field 2 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"
	
	Device_Directories=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_DIRECTORIES")
	Device_Directories=$(Field "1" "$Device_Directories")

	Device_Users=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_USERS")
	Device_Users=$(Field "1" "$Device_Users")

	echo -e "\n$(date -R) Generating symlinks for storage device $Device_Description ($Device_ID)"
	
	## Sanitize Device_Directories/Users
	if [[ $Device_Directories == "" ]]; then
		## A list containing the pluto directories
		Device_Directories=$LMCE_DIRS
	fi

	if [[ $Device_Users == "" ]]; then
		Device_Users="0"
	fi

	Device_Users=$(echo $Device_Users | sed 's/ *, */,/g')
	Device_Users=$(echo $Device_Users | sed 's/^ *//g')
	Device_Users=$(echo $Device_Users | sed 's/ *$//g')

	
	Device_Description=$(echo $Device_Description | tr '/' '-')	# Prevent / char to get in a file name

	## Count users
	countUsers=$(echo $Device_Users | sed 's/,/\n/g' | wc -l)

	## If we use a pluto directory structure
	if [[ "$Device_Users" == "-1" ]] ;then
		for mediaDir in $Device_Directories; do
		
			for userDir in /home/user_* /home/public ;do
				userDir=$(basename $userDir)

				symlinkDestination="/home/${userDir}/data/${mediaDir}/${Device_Description} [${Device_ID}]"
				symlinkSource="/mnt/device/${Device_ID}/${userDir}/data/${mediaDir}"
				
				if [[ -h "$symlinkDestination" ]] ;then
					rm -f $symlinkDestination 
				fi
				
				ln -sn "$symlinkSource" "$symlinkDestination"
				echo "$symlinkDestination -> $symlinkSource"
			done
		done

	## If the share is private without a directory structure
	else
		for j in `seq 1 $countUsers`; do
			user=$(echo $Device_Users | cut -d',' -f$j)
			
			if [[ $user == 0 ]]; then
				userDir="public/data"
			else
				userDir="user_$user/data"
			fi

			if [[ -d "/home/${userDir}" ]] ;then	
				mkdir -p "/home/${userDir}/other"
			else
				echo "Warning: /home/$userDir does not exist, skiping"
				continue
			fi

			symlinkDestination="/home/${userDir}/other/${Device_Description} [${Device_ID}]"
			symlinkSource="/mnt/device/${Device_ID}/"
			
			if [[ -h "$symlinkDestination" ]] ;then
				rm -f $symlinkDestination 
			fi

			ln -sn "$symlinkSource" "$symlinkDestination"
			echo "$symlinkDestination -> $symlinkSource"
		done
	fi

done
