#!/bin/bash
#
# Configuration script for : Device Template 1794 - Buffalo NAS HD-HG300LAN
# Configuration script for : Device Template 1837 - Windows PC or Network Storage
#
# NOTE:
#  This script will detect all the available samba or NFS shares of a nas / computer and triger
#  a device detected event for all of them

. /usr/pluto/bin/TeeMyOutput.sh --outfile /var/log/pluto/Configure_1794.log --infile /dev/null --stdboth -- "$@"
. /usr/pluto/bin/SQL_Ops.sh

DD_SHARE=126
DD_SERIAL=161

TPL_BUFFALO=1794
TPL_FILESERVER=1837
TPL_WINDOWS_SHARE=1768
TPL_NFS_SHARE=1769
TPL_INTERNAL_DRIVE=1790
TPL_RAID_0=1854
TPL_RAID_1=1851
TPL_RAID_5=1849
TPL_MEDIA_PLUGIN=2

CMD_REFRESH_LIST_OF_ONLINE_DEVICES=831

DD_SHARE_NAME=126
DD_USERNAME=127
DD_PASSWORD=128
DD_ONLINE=195
DD_UUID=267
DD_READONLY=194
DD_BLOCK_DEV=152

pid=$$

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); Device_ID="${Params[$i]}" ;;
		-i) ((i++)); Device_IP="${Params[$i]}" ;;
		-m) ((i++)); Device_MAC="${Params[$i]}" ;;
	esac
done

function SetDeviceOnline() {
	local Device_ID=$1
	local OnlineValue=$2
	local Q=""


	OldValue=$(CacheGet "$Device_ID")
	if [[ "$OldValue" == "" ]] ;then
		Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = '$Device_ID' AND FK_DeviceData='$DD_ONLINE'"
		OldValue=$(RunSQL "$Q")
		OldValue=$(Field "1" "$OldValue")
	fi

	CacheSet "$Device_ID" "$OnlineValue"
	if [[ "$OldValue" != "$OnlineValue" ]] ;then

		Q="UPDATE Device_DeviceData SET IK_DeviceData = '$OnlineValue' WHERE FK_Device = '$Device_ID' AND FK_DeviceData= '$DD_ONLINE'"
		RunSQL "$Q"

		if [[ $OnlineValue == "0" ]] ;then
			umount -lf /mnt/device/$Device_ID 2>/dev/null 1>/dev/null
		fi

		/usr/pluto/bin/MessageSend "$DCERouter" -targetType template $Device_ID $TPL_MEDIA_PLUGIN 1 $CMD_REFRESH_LIST_OF_ONLINE_DEVICES	

		Log "----------------- DEVICE $Device_ID $OnlineValue"		
	else
	
		Log "----------------- DEVICE $Device_ID $OnlineValue (unchanged)"	
	fi
}

function SetKidsOnline() {
	local Device_ID=$1
	local OnlineValue=$2
	local Q=""
	local R=""
	local Device_Child=""
	
	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia = '$Device_ID' AND FK_DeviceTemplate IN ($TPL_INTERNAL_DRIVE, $TPL_WINDOWS_SHARE, $TPL_NFS_SHARE)"
	R=$(RunCSQL "$Q")
	for Device_Child in $R ;do
		Device_Child=$(Field 1 "$Device_Child")
		SetDeviceOnline "$Device_Child" "$OnlineValue"
	done
}

function Log() {
	local Message=$1
	
	echo "$(date -R) $Message"
}

## Create a temporary mount diretory where we can test the share mounts
tempMntDir=/tmp/mnt/$pid
mkdir -p $tempMntDir

## For each fileserver device
InitCSQL "StorageDevices_FileServerRadar"
PurgeCSQL "StorageDevices_FileServerRadar"
LastRouterReload="$(stat -c %Y /usr/pluto/var/DCERouter.running)"
	if [[ "$LastRouterReload" != "$(stat -c %Y /usr/pluto/var/DCERouter.running)" ]] ;then
		LastRouterReload="$(stat -c %Y /usr/pluto/var/DCERouter.running)"
		PurgeCSQL "StorageDevices_FileServerRadar"
	fi

	## Get a list with all the File Servers from the database
	Q="
		SELECT 
			Device.PK_Device, 
			Device.IPaddress,
			Username.IK_DeviceData,
			Password.IK_DeviceData
		FROM 
			Device
			LEFT JOIN Device_DeviceData Username ON Username.FK_Device = Device.PK_Device AND Username.FK_DeviceData = '$DD_USERNAME'
			LEFT JOIN Device_DeviceData Password ON Password.FK_Device = Device.PK_Device AND Password.FK_DeviceData = '$DD_PASSWORD'
		WHERE 
			Device.FK_DeviceTemplate IN ($TPL_BUFFALO, $TPL_FILESERVER)
			
	"
	FileServerList=$(RunCSQL "$Q")

	## Test each file server
	for FileServer in $FileServerList ;do 
		Device_ID=$(Field 1 "$FileServer")
		Device_IP=$(Field 2 "$FileServer")
		Device_Username=$(Field 3 "$FileServer")
		Device_Password=$(Field 4 "$FileServer")
	
		if [[ "$Device_IP" == "" ]] ;then
			SetKidsOnline "$Device_ID" "0"
			Log "Device $Device_ID has no IP address associated with it."
			continue
		fi

		## Test to see if the file server is online or not
		ping -qnc 1 -W 1 "$Device_IP" &> /dev/null
		HostIsUp=$?
		if [[ "$HostIsUp" != "0" ]] ;then
			SetKidsOnline "$Device_ID" "0"
			Log "Device $Device_ID ($Device_IP) doesn't respond to our ping."
			continue
		fi

		
		Log "Device $Device_ID ($Device_IP) is online; looking for new shares"


	Q="SELECT MACaddress FROM Device WHERE PK_Device = '$Device_ID'"
	Device_MAC=$(RunSQL "$Q")


if [[ "$Device_MAC" == "" ]]; then
	Log "ERROR: No MAC associated with the device $Device_ID"
	continue
fi

## Get the username/pass of the device as we will use it to scan the share list and to testmount the shares
R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_USERNAME")
Device_Username=$(Field "1" "$R")
R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_PASSWORD")
Device_Password=$(Field "1" "$R")

## For every share if this is a samba server
AuthPart=" -U guest% "
if [[ "${Device_Username}" != "" ]] && [[ "$Device_Password" != "" ]] ;then
	AuthPart=" -U ${Device_Username}%${Device_Password} "
fi

if [[ "$Device_Username" != "" ]] && [[ "$Device_Password" == "" ]] ;then
	AuthPart=" -U ${Device_Username}% "
fi

for share in $(smbclient $AuthPart --list=//$Device_IP  --grepable | grep "^Disk" | cut -d'|' -f2 | tr ' ' '\\') ;do
	share=$(echo $share | tr '\\' ' ')
	pnpUID="\\\\${Device_MAC}\\${share}"
	mountedOK="false"

	## Ignore this share if we already know about it
	SQLpnpUID="\\\\\\\\${Device_MAC}\\\\${share}"
	 Q="
	SELECT
		PK_Device,
		Serial.IK_DeviceData
	FROM
		Device
		LEFT JOIN Device_DeviceData Serial ON Serial.FK_Device = Device.PK_Device AND Serial.FK_DeviceData = '$DD_SERIAL'
	WHERE
		Serial.IK_DeviceData = '$SQLpnpUID'
	AND
		FK_DeviceTemplate = '$TPL_WINDOWS_SHARE'
	"
	R=$(RunCSQL "$Q")
	ShareExists=$(Field "1" "$R")
	if [[ "$ShareExists" ]] ;then
		Log "Share $pnpUID is already known"
		continue
	fi

	Q="
	SELECT
		PK_UnknownDevices
	FROM
		UnknownDevices
	WHERE
		SerialNumber = '$SQLpnpUID'
	"
	R=$(RunCSQL "$Q")
	ShareExists=$(Field "1" "$R")
	if [[ "$ShareExists" ]] ;then
		Log "Share $pnpUID exists in UnknownDevices"
		continue
	fi

	## Try to mount it without a password
	if [[ "$mountedOK" == "false" ]] ;then
		sleep 0.2
		Log "Trying no pass"
		mount -t cifs -o username=guest,password= "//$Device_IP/$share" "$tempMntDir"
		success=$?
		if [[ "$success" == "0" ]] ;then
			umount -f -l $tempMntDir
			/usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1768 55 "182|0|$DD_SHARE|$share" 54 "$pnpUID"
			Log "Notifying router of new share $pnpUID"
			mountedOK="true"
			continue
		fi
	fi
	
	## Try to mount it with user/pass of the parent device (this device)
	if [[ "$mountedOK" == "false" ]] ;then
		sleep 0.2
		Log "Trying parent pass"
		mount -t cifs -o username=${Device_Username},password=${Device_Password} "//$Device_IP/$share" "$tempMntDir"
		success=$?
		if [[ "$success" == "0" ]] ;then
			umount -f -l $tempMntDir &>/dev/null
			/usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1768 55 "182|0|$DD_SHARE|$share|$DD_USERNAME|$Device_Username|$DD_PASSWORD|$Device_Password" 54 "$pnpUID"
			Log "Notifying router of new share $pnpUID"
			mountedOK="true"
			continue
		fi
	fi

	## Try with the username/pass of the brother devices
	if [[ "$mountedOK" == "false" ]] ;then
		Q="
			SELECT
				Username.IK_DeviceData,
				Password.IK_DeviceData
			FROM
				Device
				INNER JOIN Device_DeviceData Username ON ( Device.PK_Device = Username.FK_Device AND Username.FK_DeviceData = $DD_USERNAME ) 
				INNER JOIN Device_DeviceData Password ON ( Device.PK_Device = Password.FK_Device AND Password.FK_DeviceData = $DD_PASSWORD )
			WHERE
				FK_Device_ControlledVia = $Device_ID
		"
		R=$(RunSQL "$Q")

		siblingUserPassWorking="0"

		for UserPass in $R ;do
			Brother_Username=$(Field "1" "$UserPass")
			Brother_Password=$(Field "2" "$UserPass")
			sleep 0.2	
			Log "Trying sibling pass"
			mount -t cifs -o username=${Brother_Username},password=${Brother_Password} "//$Device_IP/$share" "$tempMntDir"
			success=$?
			if [[ "$success" == "0" ]] ;then
				umount -f -l $tempMntDir &>/dev/null
				/usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1768 55 "182|0|$DD_SHARE|$share|$DD_USERNAME|$Brother_Username|$DD_PASSWORD|$Brother_Password" 54 "$pnpUID"
				Log "Notifying router of new share $pnpUID"
				siblingUserPassWorking="1"
				mountedOK="true"
				continue
			fi
			
			[[ "$siblingUserPassWorking" == "1" ]] && break
		done
	fi

	
	## Notify the router that we didn't found any user/pass combination
	if [[ "$mountedOK" == "false" ]] ;then
			Log "Unable to mount $pnpUID, telling router to ask user for credentials"
			/usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1768 55 "182|1|$DD_SHARE|$share" 54 "$pnpUID"
	fi
done

## For every share if this is an NFS server
for share in $( showmount -e $Device_IP | grep -v Export | tr ' ' '#') ;do
	share=$(echo $share | tr '#' ' ' | awk '{print $1'})
	pnpUID="${Device_MAC}${share}"
        mountedOK="false"
        
        ## Ignore this share if we already know about it
        SQLpnpUID="${Device_MAC}${share}"
         Q="
        SELECT
                PK_Device,
                Serial.IK_DeviceData
        FROM
                Device
                LEFT JOIN Device_DeviceData Serial ON Serial.FK_Device = Device.PK_Device AND Serial.FK_DeviceData = '$DD_SERIAL'
        WHERE
                Serial.IK_DeviceData = '$SQLpnpUID'
        AND
                FK_DeviceTemplate = '$TPL_NFS_SHARE'
        "
        R=$(RunCSQL "$Q")
        ShareExists=$(Field "1" "$R")
        if [[ "$ShareExists" ]] ;then
                Log "Share $pnpUID is already known"
                continue
        fi

        Q="
        SELECT
                PK_UnknownDevices
        FROM
                UnknownDevices
        WHERE
                SerialNumber = '$SQLpnpUID'
        "
        R=$(RunCSQL "$Q")
        ShareExists=$(Field "1" "$R")
        if [[ "$ShareExists" ]] ;then
                Log "Share $pnpUID exists in UnknownDevices"
                continue
        fi

        ## Try to mount it
        if [[ "$mountedOK" == "false" ]] ;then
                sleep 0.2
                mount -t nfs "$Device_IP:$share" "$tempMntDir"
                success=$?
                echo "$(date -R) mount -t nfs \"$Device_IP:$share\" \"$tempMntDir\" [$success]"

                if [[ "$success" == "0" ]] ;then
                        umount -f -l $tempMntDir
                        /usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1769 55 "182|0|$DD_SHARE|$share" 54 "$pnpUID"
                        Log "Notifying router of new share $pnpUID"
                        mountedOK="true"
                fi
        fi
done
done

rmdir $tempMntDir
