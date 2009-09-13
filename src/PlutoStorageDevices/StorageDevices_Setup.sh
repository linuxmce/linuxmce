#!/bin/bash

## TrigerCascade="true" result in script being run on the other computers too
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Utils.sh
AutoMaster_StorageDevices="/mnt/device /etc/auto.PlutoStorageDevices --timeout=30"
PopulateSection "/etc/auto.master" "PlutoStorageDevices" "$AutoMaster_StorageDevices"
mkdir -p /mnt/device


TrigerCascade="true"

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                --cascade) ((i++)); TrigerCascade="true" ;;
                --nocascade) ((i++)); TrigerCascade="false" ;;
        esac
done

## Start autmounter
if [[ -x /etc/init.d/autofs ]]; then
	if [[ "$(pidof automount)" == "" ]] ;then
		invoke-rc.d autofs start
	else
		invoke-rc.d autofs reload
	fi
else
	echo "WARNING: Autmount daemon doesn't exist ?!"
fi


## Exports our local storage devices so other computers can access them (runs on every computer)
/usr/pluto/bin/StorageDevices_ExportsNFS.sh

## Export our local storage device over samba too
/usr/pluto/bin/StorageDevices_ExportsSMB.sh

## Creates a Pluto Directory structure on local devices (runs on every computer)
/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh

## Creates symlinks to storage device in /home (runs only on core)
/usr/pluto/bin/StorageDevices_Symlinks.sh

## Start UPNP media files client
mkdir -p /mnt/upnp
if ! mountpoint /mnt/upnp ;then
	if [ -x /usr/bin/djmount ] ;then
		djmount -o allow_other,default_permissions /mnt/upnp
	fi
fi

# Call MythTV Setup to alter storage groups. 
/usr/pluto/bin/mythtv_setup.pl

## Call this script on the other machines too
if [[ $TrigerCascade == "true" ]] ;then
	/usr/pluto/bin/MessageSend "$DCERouter" -targetType template 0 26 1 67 13 "/usr/pluto/bin/StorageDevices_Setup.sh" 51 "--nocascade"
fi

