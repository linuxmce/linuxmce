#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh

## We should not run un diskless mds
if [[ -f /etc/diskless.conf ]]; then
	exit 0
fi

## Regenerate the CommonDiskless Section of /etc/exports
INTERNAL_SUBNET_MASK="$IntNetmask"
INTERNAL_ADDRESS="$IntIP"

for i in 1 2 3 4; do
	IPDigit=$(echo $INTERNAL_ADDRESS | cut -d. -f$i)
	MaskDigit=$(echo $INTERNAL_SUBNET_MASK | cut -d. -f$i)
	NetDigit=$(($IPDigit & $MaskDigit))
	INTERNAL_SUBNET="$INTERNAL_SUBNET$Dot$NetDigit" && Dot="."
done
				
Exports_CommonDiskless="
/home                   $INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(rw,no_root_squash,no_all_squash,sync,no_subtree_check)
/usr/pluto/orbiter      $INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(rw,no_root_squash,no_all_squash,sync,no_subtree_check)
/usr/pluto/keys         $INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(ro,no_root_squash,no_all_squash,sync,no_subtree_check)
/usr/pluto/deb-cache    $INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(ro,no_root_squash,no_all_squash,sync,no_subtree_check)
/usr/pluto/var          $INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(rw,no_root_squash,no_all_squash,sync,no_subtree_check)
"
PopulateSection "/etc/exports" "CommonDiskless" "$Exports_CommonDiskless"

## Regenerate the DisklessMDRoots Section of /etc/exports
Q="
    SELECT 
    	PK_Device,
	IPaddress
    FROM
    	Device
    	JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
    WHERE 
    	PK_DeviceTemplate=28 
	AND 
	FK_Device_ControlledVia IS NULL
"
DisklessMDs=$(RunSQL "$Q")


DisklessFileSystemsLocation="/usr/pluto/diskless"

for Client in $DisklessMDs; do
       	DisklessMD_ID=$(Field 1 "$Client")
       	DisklessMD_IP=$(Field 2 "$Client")

        if [[ -z "$DisklessMD_IP" ]]; then
                echo "*** WARNING *** Diskless MD $DisklessMD_ID is not associated to any Ip address"
		continue
        fi

	DisklessMD_Root="$DisklessFileSystemsLocation/$DisklessMD_ID"

	Q="
	    SELECT PK_Device
	    FROM Device_DeviceData
	    JOIN Device ON PK_Device=FK_Device
	    JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	    WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IK_DeviceData='1' AND PK_Device='$DisklessMD_ID'
	"
	IsDiskless=$(RunSQL "$Q")

	if [ -n "$IsDiskless" ]; then
		Exports_DisklessMDRoots="$Exports_DisklessMDRoots\n$DisklessMD_Root	$INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(rw,no_root_squash,no_all_squash,sync,no_subtree_check)"
	fi
done

PopulateSection "/etc/exports" "DisklessMDRoots" "$Exports_DisklessMDRoots"

## Check and start/reload the nfs-kernel-server
if [[ "$(pidof rpc.mountd)" == "" ]] ;then
        service nfs-kernel-server restart
fi
service nfs-kernel-server reload
