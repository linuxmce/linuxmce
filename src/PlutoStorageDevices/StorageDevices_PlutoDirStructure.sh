#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh



TPL_STORAGE_DEVICES="1790, 1794, 1768, 1769, 1854, 1851, 1849, 2351, 2352, 2353, 2354, 2355"
DD_USERS=3

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); CommaSeparatedDeviceList="${Params[$i]}" ;;
        esac
done


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

echo "Creating /home/public paths..."
##lets handle the /home/public paths....
##For every directory
for mediaDir in $LMCE_DIRS ;do
	
	## For every user
	for User in $Users; do
		User_ID=$(Field 1 "$User")
		User_Uname=$(Field 2 "$User")
		User_UnixUname=$(Field 2 "$User" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
		User_UnixUname="pluto_$User_UnixUname"
		
		mkdir -p /home/user_$User_ID/data/$mediaDir
		chown $User_UnixUname:$User_UnixUname /home/user_$User_ID/data/$mediaDir
		chmod 2770 /home/user_$User_ID/data/$mediaDir
	done

	mkdir -p /home/public/data/$mediaDir
	chown root:public /home/public/data/$mediaDir
	chmod 2775 /home/public/data/$mediaDir

done

echo "Creating paths for each extra storage device..."
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

	##For every directory
	for mediaDir in $LMCE_DIRS ;do

		## one for public
		mkdir -p $Device_MountPoint/public/data/$mediaDir
		chown root:public $Device_MountPoint/public/data/$mediaDir
		chmod 2775 $Device_MountPoint/public/data/$mediaDir
		
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
	done
done



