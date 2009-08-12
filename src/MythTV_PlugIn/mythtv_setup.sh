#!/bin/bash

# We execute the perl script instead of this shell script
# due to problems with escaping strings
/usr/bin/perl /usr/pluto/bin/mythtv_setup.pl
exit 0

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

#Create a way to determine if MythTV is installed...
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=36"
MythTV_Installed=$(RunSQL "$Q")

function ClearMythTVStorageGroups
{
	#Exit if mythtv is not installed
	if [ -z $MythTV_Installed ];then
        	return 0
	fi

	echo "Cleaning out MythTV Storage Groups..."
	#Allow users to have custom storage groups. Lets store them now so we can restore them after clearing the storagegroups table
	UseDB "mythconverg"
	Q="SELECT groupname,hostname,dirname FROM storagegroup WHERE groupname LIKE 'custom:%'"
	customSG=$(RunSQL "$Q")

	#Clear the table.. This also resets th auto-increment number
	Q="TRUNCATE TABLE storagegroup"
	RunSQL "$Q"

	#restore custom storage groups.
	for SG in $customSG; do
		groupName=$(Field 1 "$SG")
		hostName=$(Field 2 "$SG")
		dirName=$(Field 3 "$SG")

		echo "Retaining custom storagegroup:     '$groupName'"
		Q="INSERT INTO storagegroup (groupname,hostname,dirname) VALUES ('$groupName','$hostName','$dirName')"
		RunSQL "$Q"
	done
}
function CleanMythTVSettings
{
	# The mythconverg.settings table may contain entries for hosts that no longer exist... This can happen if a user deletes a MD device.
	# This function will remove entries from the settings table where the host no longer exists...

	#Exit if mythtv is not installed
	if [ -z $MythTV_Installed ];then
        	return 0
	fi

	echo "Performing cleanup of mythtv.settings table..."

	Q="SELECT p.PK_Device FROM pluto_main.Device p 
	LEFT JOIN pluto_main.DeviceTemplate p2 ON p2.PK_DeviceTemplate = p.FK_DeviceTemplate
	WHERE (p2.FK_DeviceCategory=7 OR p2.FK_DeviceCategory=8) AND p.FK_Device_ControlledVia IS Null AND PK_Device!=1"
	UseDB "pluto_main"
	moons=$(RunSQL "$Q")
	hostNames="'localhost','dcerouter'"
	for moon in $moons; do
		hostNames="$hostNames,'moon$(Field 1 "$moon")'"
	done

	Q="DELETE FROM settings WHERE hostname NOT IN($hostNames)"
	UseDB "mythconverg"
	RunSQL "$Q"
}
function AddMythTVStorageGroup
{
	## Parameters 
	## 1 - path
	## 2 - groupname
	path=$1
	name=$2
	host=$3

	#Exit if mythtv is not installed
	if [ -z $MythTV_Installed ];then
        	return 0
	fi
	
	echo "Adding MythTV storage group $name for host $host"
	Q="INSERT INTO mythconverg.storagegroup (groupname,hostname,dirname) VALUES ('$name','$host','$path')"
	#echo "$Q"
	UseDB "mythconverg"
	RunSQL "$Q"
}
CleanMythTVSettings

TPL_STORAGE_DEVICES="1790, 1794, 1768, 1769, 1854, 1851, 1849"
DD_USERS=3

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); CommaSeparatedDeviceList="${Params[$i]}" ;;
        esac
done


## A list containing the pluto users that need to use those directories
Q="SELECT PK_Users, UserName FROM Users"
UseDB "pluto_main"
Users=$(RunSQL "$Q")

## Get a list of storage devices that use the pluto dir structure
if [[ "$CommaSeparatedDeviceList" == "" ]]; then
	Q="
		SELECT 
		Device.PK_Device,
		Device.Description
	FROM 
		Device 
		JOIN Device_DeviceData ON FK_Device = PK_Device
	WHERE 
		FK_DeviceTemplate IN ($TPL_STORAGE_DEVICES)
		AND
		FK_DeviceData = $DD_USERS
		AND
		IK_DeviceData LIKE '%-1%'
		AND
		FK_Device_ControlledVia = '$PK_Device'
	ORDER BY PK_Device
	"
	UseDB "pluto_main"
	Devices=$(RunSQL "$Q")
else
        Q="
                SELECT
                PK_Device,
		Description
        FROM
                Device
                JOIN Device_DeviceData ON FK_Device = PK_Device
        WHERE
                FK_DeviceTemplate IN ($TPL_STORAGE_DEVICES)
                AND
                FK_DeviceData = $DD_USERS
                AND
                IK_DeviceData LIKE '%-1%'
                AND
		PK_Device IN ($CommaSeparatedDeviceList)
	 ORDER BY PK_Device
        "
	UseDB "pluto_main"
	Devices=$(RunSQL "$Q")
fi

## Make a list of hostnames
Q="SELECT p.PK_Device FROM pluto_main.Device p 
LEFT JOIN pluto_main.DeviceTemplate p2 ON p2.PK_DeviceTemplate = p.FK_DeviceTemplate
WHERE (p2.FK_DeviceCategory=7 OR p2.FK_DeviceCategory=8) AND p.FK_Device_ControlledVia IS Null AND PK_Device!=1"
UseDB "pluto_main"
moons=$(RunSQL "$Q")
hostNames="dcerouter"
for moon in $moons; do
	hostNames="$hostNames moon$(Field 1 "$moon")"
done

# Exit gracefully if no new MD's or storage devices were added.
# This makes sure that we don't have to reset the backend more than we should have to...
UpdateStorageGroups=1
for thisHost in $hostNames; do
	Q="SELECT hostname FROM storagegroup WHERE hostname='$thisHost'"
	UseDB "mythconverg"
	resHosts=$(RunSQL "$Q")
	if [ -z $resHost ]; then
		UpdateStorageGroups=0
	fi
done
for thisDevice in $Devices; do
	Device_Description=$(Field 2 "$Device")
	Q="SELECT id from storagegroup WHERE dirname LIKE '%$Device_Description%'"
	UseDB "mythconverg"
	resHDevices=$(RunSQL "$Q")
	if [ -z $resHost ]; then
		UpdateStorageGroups=0
	fi
done
#echo "$UpdateStorageGroups"
if [ UpdateStorageGroups==0 ]; then
	echo "No new media directors or storage devices found, skipping storagegroup update and backend reload..."
	exit 0
fi


## Something has changed, so we must update the storagegroups and reset the backends!!
## Before we start, lets clean out mythconverg.storagegroups
ClearMythTVStorageGroups

##For every host...
for hostName in $hostNames; do

	##lets handle the /home/public paths....
	AddMythTVStorageGroup "/home/public/data/pvr" "Default" "$hostName"     #Put the special "Default" storage group in. 
	AddMythTVStorageGroup "/home/public/data/pvr" "LiveTV" "$hostName"    #Put the special "LiveTV" storage group into the moons root tv_shows_* directory	
	AddMythTVStorageGroup "/home/public/data/pvr" "public" "$hostName"	


	## For every user
	for User in $Users; do
		User_ID=$(Field 1 "$User")
		User_Uname=$(Field 2 "$User")
		User_UnixUname=$(Field 2 "$User" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
		User_UnixUname="pluto_$User_UnixUname"
	
		AddMythTVStorageGroup "/home/user_$User_ID/data/pvr" "$User_Uname" "$hostName"
	done



	## And now add the directories to each extra storage device.
	for Device in $Devices; do
		Device_ID=$(Field 1 "$Device")
		Device_Description=$(Field 2 "$Device")

		Device_MountPoint="/mnt/device/$Device_ID"

		Device_IsMounted=$(cd /mnt/device/$Device_ID && mount | grep "\/mnt\/device\/$Device_ID ") 
		if [[ "$Device_IsMounted" == "" ]]; then 
		        echo "WARNING: Device $Device_ID is not mounted, skiping ..." 
		        continue 
		fi 

		#public storage groups
		AddMythTVStorageGroup "/home/public/data/pvr/$Device_Description [$Device_ID]" "Default" "$hostName"      #Put the special "Default" storage group in. 
		AddMythTVStorageGroup "/home/public/data/pvr/$Device_Description [$Device_ID]" "LiveTV" "$hostName"       #Put the special "LiveTV" storage group into the moons root tv_shows_* directory	
		AddMythTVStorageGroup "/home/public/data/pvr/$Device_Description [$Device_ID]" "public: $Device_Description [$Device_ID]" "$hostName"


		## For every user
		for User in $Users; do
			User_ID=$(Field 1 "$User")
			User_Uname=$(Field 2 "$User")
			User_UnixUname=$(Field 2 "$User" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
			User_UnixUname="pluto_$User_UnixUname"
		
			AddMythTVStorageGroup "/home/user_$User_ID/data/pvr/$Device_Description [$Device_ID]" "$User_Uname: $Device_Description [$Device_ID]" "$hostName"

		done
	done
done

#Reset the MythTV Backend so that the new storage groups are available
if [ $MythTV_Installed ];then
        /usr/pluto/bin/Restart_Backend_With_SchemaLock.sh
fi
