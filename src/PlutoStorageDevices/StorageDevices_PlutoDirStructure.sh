#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

#Create a way to determine if MythTV is installed...
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=36"
MythTV_Installed=$(RunSQL "$Q")

function ClearMythTVStorageGroups
{
	UseDB "mythconverg"
	echo "Cleaning out MythTV Storage Groups..."
	Q="TRUNCATE TABLE storagegroup"
	RunSQL "$Q"
}

function AddMythTVStorageGroup
{
	## Parameters 
	## 1 - path
	## 2 - groupname
	path=$1
	name=$2

	#Exit if mythtv is not installed
	if [ -z $MythTV_Installed ];then
        	return 0
	fi

	
	#Exit if the path does not contain tv_shows_*!!
	tv_dir=${path##*videos/}
	pk_dev=${tv_dir##*_}
	pk_dev=${pk_dev%/*}
	contains_tv_shows_=${tv_dir%_*}
	if [ "$contains_tv_shows_" != "tv_shows" ] ; then
		return 0
	fi

	
	echo "Adding storage group $name"
	#echo "$contains_tv_shows_"
	#echo "$path"
	#echo "$name"
	Q="SELECT p.PK_Device,p.IPaddress,m.hostname FROM pluto_main.Device p 
	LEFT JOIN pluto_main.DeviceTemplate p2 ON p2.PK_DeviceTemplate = p.FK_DeviceTemplate 
	LEFT JOIN mythconverg.settings m ON m.data = p.IPaddress
	WHERE p.PK_Device="$pk_dev" AND m.hostname IS NOT NULL"
	UseDB "pluto_main"
	res=$(RunSQL "$Q")
	# echo "$res"

	
	Q="INSERT INTO mythconverg.storagegroup (groupname,hostname,dirname) VALUES ('$name','$(Field 3 "$res")','$path')"
	#echo "$Q"
	UseDB "mythconverg"
	RunSQL "$Q"
}

TPL_STORAGE_DEVICES="1790, 1794, 1768, 1769, 1854, 1851, 1849"
DD_USERS=3

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); CommaSeparatedDeviceList="${Params[$i]}" ;;
        esac
done

## A list containing the pluto directories
Directories="pictures,audio,documents,videos,games/MAME"
## Add in the tv_shows_* directories, and get information such as IPAddress vs. Hostname if MythTV is installed
if [ $MythTV_Installed ];then
	Q="SELECT p.PK_Device,p.IPaddress, m.hostname FROM pluto_main.Device p 
	LEFT JOIN pluto_main.DeviceTemplate p2 ON p2.PK_DeviceTemplate = p.FK_DeviceTemplate ##For every directory
	RIGHT JOIN mythconverg.settings m ON m.data = p.IPaddress
	WHERE (p2.FK_DeviceCategory=7 OR p2.FK_DeviceCategory=8) AND p.FK_Device_ControlledVia IS Null AND m.hostname IS NOT NULL"
	UseDB "pluto_main"
	deviceList=$(RunSQL "$Q")
	for thisDevice in $deviceList; do
		Directories=$Directories,videos/tv_shows_$(Field 1 "$thisDevice")
		IPAddresses=$IPAddresses,$(Field 2 "$thisDevice")
		HostNames=$HostNames,$(Field 3 "$thisDevice")
	done
	IPAddresses=${IPAddresses#,}
	HostNames=${HostNames#,}
fi
## Done adding tv_shows_* directories

countDirs=$(echo $Directories | sed 's/,/\n/g' | wc -l)

## A list containin the pluto users that need to use those directories
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

## Lets get the device with the most storage space so we can set up the default mythtv storage groups
Q="SELECT FK_Device
FROM Device_DeviceData
WHERE IK_DeviceData=
(SELECT MAX(IK_DeviceData) FROM Device_DeviceData WHERE FK_DeviceData=160)"
UseDB "pluto_main"
freespace=$(RunSQL "$Q")
hasMostFreespace=$(Field 1 "$freespace")

## Before we start, lets clean out mythconverg.storagegroups
ClearMythTVStorageGroups

##lets handle the /home/public paths....
##For every directory
for i in `seq 1 $countDirs` ;do
	mediaDir=$(echo $Directories | cut -d',' -f$i)

	
	AddMythTVStorageGroup "/home/public/data/$mediaDir" "Default"      #Put the special "Default" storage group in. It will be removed and overwritten below if there is a device with more freespace
	AddMythTVStorageGroup "/home/public/data/$mediaDir" "LiveTV"       #Put the special "LiveTV" storage group into the moons root tv_shows_* directory	
	AddMythTVStorageGroup "/home/public/data/$mediaDir" "public"	

	## For every user
	for User in $Users; do
		User_ID=$(Field 1 "$User")
		User_Uname=$(Field 2 "$User")
		User_UnixUname=$(Field 2 "$User" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
		User_UnixUname="pluto_$User_UnixUname"
		
		mkdir -p /home/user_$User_ID/data/$mediaDir
		chown $User_UnixUname:$User_UnixUname /home/user_$User_ID/data/$mediaDir
		chmod 2770 /home/user_$User_ID/data/$mediaDir
		AddMythTVStorageGroup "/home/user_$User_ID/data/$mediaDir" "user_$User_ID"
		
	done

	mkdir -p /home/public/data/$mediaDir
	chown root:public /home/public/data/$mediaDir
	chmod 2775 /home/public/data/$mediaDir

done


## And now add the user directories
for Device in $Devices; do
	Device_ID=$(Field 1 "$Device")
	Device_Description=$(Field 2 "$Device")

	Device_MountPoint="/mnt/device/$Device_ID"

	Device_IsMounted=$(cd /mnt/device/$Device_ID && mount | grep "\/mnt\/device\/$Device_ID ")
	if [[ "$Device_IsMounted" == "" ]]; then
		echo "WARNING: Device $Device_ID is not mounted, skiping ..."
		continue
	fi

	##For every directory
	for i in `seq 1 $countDirs` ;do
		mediaDir=$(echo $Directories | cut -d',' -f$i)

		## one for public
		mkdir -p $Device_MountPoint/public/data/$mediaDir
		chown root:public $Device_MountPoint/public/data/$mediaDir
		chmod 2775 $Device_MountPoint/public/data/$mediaDir
		#Update the default if needed
		if [ $Device_ID == $hasMostFreespace ]; then

			#This device has the most freespace, lets update the Default storage group for it	
			Q="UPDATE storagegroup SET dirname='/home/user_${User_ID}/data/${mediaDir}/${Device_Description} [${Device_ID}]' WHERE groupname = 'Default' AND dirname='/home/public/data/$mediaDir'"
			UseDB "mythconverg"
			RunSQL "$Q"
		fi
		#public storage group
		AddMythTVStorageGroup "home/public/data/$mediaDir/$Device_Description [$Device_ID]" "public/$Device_Description [$Device_ID]"

		## For every user
		for User in $Users; do
			User_ID=$(Field 1 "$User")
			User_Uname=$(Field 2 "$User")
			User_UnixUname=$(Field 2 "$User" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
			User_UnixUname="pluto_$User_UnixUname"
			
			mkdir -p $Device_MountPoint/user_$User_ID/data/$mediaDir
			chown $User_UnixUname:$User_UnixUname $Device_MountPoint/user_$User_ID/data/$mediaDir
			chmod 2770 $Device_MountPoint/user_$User_ID/data/$mediaDir
			AddMythTVStorageGroup "home/user_$User_ID/data/$mediaDir/$Device_Description [$Device_ID]" "user_$User_ID/$Device_Description [$Device_ID]"	
		done
	done
done

#Reset the MythTV Backend so that new storage groups are available
if [ $MythTV_Installed ];then
        /usr/pluto/bin/Restart_Backend_With_SchemaLock.sh
fi


