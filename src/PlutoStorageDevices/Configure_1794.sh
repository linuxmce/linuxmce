#!/bin/bash
#
# Configuration script for : Device Template 1794 - Buffalo NAS HD-HG300LAN
# Configuration script for : Device Template 1837 - Windows PC or Network Storage
#
# NOTE:
#  This script will detect all the available samba or NFS shares of a nas / computer and triger
#  a device detected event for all of them

. /usr/pluto/bin/TeeMyOutput.sh --outfile /var/log/pluto/Configure_1794.log --infile /dev/null --stdboth -- "$@"

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DD_USERNAME=127
DD_PASSWORD=128
DD_SHARE=126

pid=$$

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); Device_ID="${Params[$i]}" ;;
		-i) ((i++)); Device_IP="${Params[$i]}" ;;
		-m) ((i++)); Device_MAC="${Params[$i]}" ;;
	esac
done

echo "$(date -R) Called to configure Device : $Device_ID"

## Check PK_Device
if [[ "$Device_ID" == "" ]]; then
	echo "ERROR: No PK_Device assigned to this device"
fi

## Check Device ip address
if [[ "$Device_IP" == "" ]]; then
	Q="SELECT IPaddress FROM Device WHERE PK_Device = '$Device_ID'"
	Device_IP=$(RunSQL "$Q")
fi

if [[ "$Device_IP" == "" ]]; then
	echo "ERROR: No IP associated with the device $Device_ID"
	exit 1
fi

if [[ "$Device_MAC" == "" ]]; then
	Q="SELECT MACaddress FROM Device WHERE PK_Device = '$Device_ID'"
	Device_MAC=$(RunSQL "$Q")
fi

if [[ "$Device_MAC" == "" ]]; then
	echo "ERROR: No MAC associated with the device $Device_ID"
	exit 1
fi

## Get the username/pass of the device as we will use it to scan the share list and to testmount the shares
R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_USERNAME")
Device_Username=$(Field "1" "$R")
R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_PASSWORD")
Device_Password=$(Field "1" "$R")

## Create a temporary mount diretory where we can test the share mounts
tempMntDir=/tmp/mnt/$pid
mkdir -p $tempMntDir

## For every share if this is a samba server
AuthPart=" -U guest% "
if [[ "${Device_Username}" != "" ]] && [[ "$Device_Password" != "" ]] ;then
	AuthPart=" -U ${Device_Username}%${Device_Password} "
fi

if [[ "$Device_Username" != "" ]] && [[ "$Device_Password" == "" ]] ;then
	AuthPart=" -U ${Device_Username}% "
fi

for share in $(smbclient $AuthPart --list=//$Device_IP  --grepable | grep "^Disk" | cut -d'|' -f2 | tr ' ' '\\') ;do
	echo 
	echo
	share=$(echo $share | tr '\\' ' ')
	pnpUID="\\\\${Device_MAC}\\${share}"
	mountedOK="false"
	echo "$(date -R) Checking $pnpUID"

	## Try to mount it without a password
	if [[ "$mountedOK" == "false" ]] ;then
		sleep 0.2
		mount -t cifs -o username=guest,password= "//$Device_IP/$share" "$tempMntDir"
		success=$?
		echo "$(date -R) mount -t cifs -o username=guest,password= \"//$Device_IP/$share\" \"$tempMntDir\" [$success]" 
		
		if [[ "$success" == "0" ]] ;then
			umount -f -l $tempMntDir
			/usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1768 55 "182|0|$DD_SHARE|$share" 54 "$pnpUID"
			mountedOK="true"
		fi
	fi
	
	## Try to mount it with user/pass of the parent device (this device)
	if [[ "$mountedOK" == "false" ]] ;then
		sleep 0.2
		mount -t cifs -o username=${Device_Username},password=${Device_Password} "//$Device_IP/$share" "$tempMntDir"
		success=$?
		echo "$(date -R) mount -t cifs -o username=${Device_Username},password=${Device_Password} \"//$Device_IP/$share\" \"$tempMntDir\" [$success]"
		
		if [[ "$success" == "0" ]] ;then
			umount -f -l $tempMntDir &>/dev/null
			/usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1768 55 "182|0|$DD_SHARE|$share|$DD_USERNAME|$Device_Username|$DD_PASSWORD|$Device_Password" 54 "$pnpUID"
			mountedOK="true"
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
			mount -t cifs -o username=${Brother_Username},password=${Brother_Password} "//$Device_IP/$share" "$tempMntDir"
			success=$?
			echo "$(date -R) mount -t cifs -o username=${Brother_Username},password=${Brother_Password} \"//$Device_IP/$share\" \"$tempMntDir\" [$success]"

			if [[ "$success" == "0" ]] ;then
				umount -f -l $tempMntDir &>/dev/null
				/usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1768 55 "182|0|$DD_SHARE|$share|$DD_USERNAME|$Brother_Username|$DD_PASSWORD|$Brother_Password" 54 "$pnpUID"

				siblingUserPassWorking="1"
				mountedOK="true"
			fi
			
			[[ "$siblingUserPassWorking" == "1" ]] && break
		done
	fi

	
	## Notify the router that we didn't found any user/pass combination
	if [[ "$mountedOK" == "false" ]] ;then
		echo "$(date -R) Asking for password for '$share'"
		/usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1768 55 "182|1|$DD_SHARE|$share" 54 "$pnpUID"
	fi
done

## For every share if this is an NFS server
for share in $( showmount -e $Device_IP | tr ' ' '#') ;do
	echo
	echo
	share=$(echo $share | tr '#' ' ' | awk '{print $1'})
	pnpUID="${Device_MAC}${share}"
        mountedOK="false"
        echo "$(date -R) Checking $pnpUID"

        ## Try to mount it
        if [[ "$mountedOK" == "false" ]] ;then
                sleep 0.2
                mount -t nfs "$Device_IP:$share" "$tempMntDir"
                success=$?
                echo "$(date -R) mount -t nfs \"$Device_IP:$share\" \"$tempMntDir\" [$success]"

                if [[ "$success" == "0" ]] ;then
                        umount -f -l $tempMntDir
                        /usr/pluto/bin/MessageSend "$DCERouter" $Device_ID -1001 2 65 56 "fileshare" 52 3 53 2 49 1769 55 "182|0|$DD_SHARE|$share" 54 "$pnpUID"
                        mountedOK="true"
                fi
        fi
done

rmdir $tempMntDir

/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh -d $Device_ID
/usr/pluto/bin/StorageDevices_Setup.sh --cascade

