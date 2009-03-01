#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh


TPL_STORAGE_DEVICES="1790, 1794, 1768, 1769, 1854, 1851, 1849"
DD_USERS=3

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); CommaSeparatedDeviceList="${Params[$i]}" ;;
        esac
done

## A list containing the plugo directories
Directories="pictures,audio,documents,videos,games/MAME"
countDirs=$(echo $Directories | sed 's/,/\n/g' | wc -l)

## A list containin the pluto users that need to use those directories
Q="SELECT PK_Users, UserName FROM Users"
Users=$(RunSQL "$Q")

## Get a list of storage devices that use the pluto dir structure
if [[ "$CommaSeparatedDeviceList" == "" ]]; then
	Q="
		SELECT 
		PK_Device 
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
	"
	Devices=$(RunSQL "$Q")
else
        Q="
                SELECT
                PK_Device
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
        "
	Devices=$(RunSQL "$Q")
fi


for Device in $Devices; do
	Device_ID=$(Field 1 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"

	Device_IsMounted=$(cd /mnt/device/$Device_ID && mount | grep "\/mnt\/device\/$Device_ID ")
	if [[ "$Device_IsMounted" == "" ]]; then
		echo "WARNING: Device $Device_ID is not mounted, skiping ..."
		continue
	fi

	##For every directory
	for i in `seq 1 $countDirs` ;do
		mediaDir=$(echo $Directories | cut -d',' -f$i)
		
		## For every user
		for User in $Users; do
			User_ID=$(Field 1 "$User")
			User_Uname=$(Field 2 "$User")
			User_UnixUname=$(Field 2 "$User" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
			User_UnixUname="pluto_$User_UnixUname"
			
			mkdir -p $Device_MountPoint/user_$User_ID/data/$mediaDir
			chown $User_UnixUname:$User_UnixUname $Device_MountPoint/user_$User_ID/data/$mediaDir
			chmod 2770 $Device_MountPoint/user_$User_ID/data/$mediaDir
		done

		## And one for public
		mkdir -p $Device_MountPoint/public/data/$mediaDir
		chown root:public $Device_MountPoint/public/data/$mediaDir
		chmod 2775 $Device_MountPoint/public/data/$mediaDir
	done
done
