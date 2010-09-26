#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Utils.sh

TPL_GENERIC_INTERNAL_DRIVE=1790
TPL_RAID_0=1854
TPL_RAID_1=1851
TPL_RAID_5=1849

## Set some variables needed for exporting nfs
if [[ -f /etc/diskless.conf ]] ;then
        INTERNAL_ADDRESS="$ExtIP"
        INTERNAL_SUBNET_MASK="$ExtNetmask"
else
        INTERNAL_SUBNET_MASK="$IntNetmask"
        INTERNAL_ADDRESS="$IntIP"
fi

INTERNAL_SUBNET=""
for i in 1 2 3 4; do
	IPDigit=$(echo $INTERNAL_ADDRESS | cut -d. -f$i)
	MaskDigit=$(echo $INTERNAL_SUBNET_MASK | cut -d. -f$i)
	NetDigit=$(($IPDigit & $MaskDigit))
	INTERNAL_SUBNET="$INTERNAL_SUBNET$Dot$NetDigit" && Dot="."
done
				

## Lookup our internal storage devices in the db
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate IN ($TPL_GENERIC_INTERNAL_DRIVE,$TPL_RAID_0,$TPL_RAID_1,$TPL_RAID_5) AND FK_Device_ControlledVia=$PK_Device"
InternalOwnStorageDevices=$(RunSQL "$Q")

for Device in $InternalOwnStorageDevices; do
	Device_ID=$(Field 1 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"

	Exports_InternalStorageDevices="$Exports_InternalStorageDevices\n$Device_MountPoint ${INTERNAL_SUBNET}/${INTERNAL_SUBNET_MASK}(rw,no_root_squash,no_all_squash,async,nohide,no_subtree_check)"
done

if ! BlacklistConfFiles '/etc/exports' ;then
	if [ -e /etc/exports ] && [ ! -e /etc/exports.pbackup ] ;then
		cp /etc/exports /etc/exports.pbackup
	fi
	PopulateSection "/etc/exports" "InternalStorageDevices" "$Exports_InternalStorageDevices"
fi

## Check and start/reload the nfs-kernel-server
if [[ "$(pidof rpc.mountd)" == "" ]] ;then
	invoke-rc.d nfs-kernel-server restart
fi
invoke-rc.d nfs-kernel-server reload
