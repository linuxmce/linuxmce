#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Utils.sh

DEVICEDATA_DisklessImages=258

## We should not run on diskless mds
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

INTERNAL_CIDR_NOTATION=0
IFS=.
for dec in $INTERNAL_SUBNET_MASK ; do
	case $dec in
		255) let INTERNAL_CIDR_NOTATION+=8;;
		254) let INTERNAL_CIDR_NOTATION+=7;;
		252) let INTERNAL_CIDR_NOTATION+=6;;
		248) let INTERNAL_CIDR_NOTATION+=5;;
		240) let INTERNAL_CIDR_NOTATION+=4;;
		224) let INTERNAL_CIDR_NOTATION+=3;;
		192) let INTERNAL_CIDR_NOTATION+=2;;
		128) let INTERNAL_CIDR_NOTATION+=1;;
		0);;
		*) echo "Error: $dec is not recognised"; exit 1
	esac
done
unset IFS
				
Exports_CommonDiskless="
/                       $INTERNAL_SUBNET/$INTERNAL_CIDR_NOTATION(fsid=0,rw,no_root_squash,no_all_squash,sync,no_subtree_check)
/home                   $INTERNAL_SUBNET/$INTERNAL_CIDR_NOTATION(rw,no_root_squash,no_all_squash,sync,no_subtree_check)
/usr/pluto/orbiter      $INTERNAL_SUBNET/$INTERNAL_CIDR_NOTATION(ro,no_root_squash,no_all_squash,sync,no_subtree_check)
/usr/pluto/keys         $INTERNAL_SUBNET/$INTERNAL_CIDR_NOTATION(ro,no_root_squash,no_all_squash,sync,no_subtree_check)
/usr/pluto/deb-cache    $INTERNAL_SUBNET/$INTERNAL_CIDR_NOTATION(ro,no_root_squash,no_all_squash,sync,no_subtree_check)
/usr/pluto/var          $INTERNAL_SUBNET/$INTERNAL_CIDR_NOTATION(rw,no_root_squash,no_all_squash,sync,no_subtree_check)
/var/spool/asterisk     $INTERNAL_SUBNET/$INTERNAL_CIDR_NOTATION(rw,no_root_squash,no_all_squash,sync,no_subtree_check)
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
	FK_DeviceCategory=8
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
	    WHERE FK_DeviceCategory=8 AND FK_DeviceData=9 AND IK_DeviceData='1' AND PK_Device='$DisklessMD_ID'
	"
	IsDiskless=$(RunSQL "$Q")

	if [ -n "$IsDiskless" ]; then
		DisklessImages=$(GetDeviceData "$DisklessMD_ID" "$DEVICEDATA_DisklessImages")
		if [[ -z "$DisklessImages" ]]; then
			Exports_DisklessMDRoots="$Exports_DisklessMDRoots\n$DisklessMD_Root	$INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(rw,no_root_squash,no_all_squash,sync,no_subtree_check)"
		else
			for line in $DisklessImages; do
				NameKernel="${line%%=*}"
				if [[ "$NameKernel" == *-* ]]; then
					Name="${NameKernel%%-*}"
					Kernel="${NameKernel#*-}"
				else
					Name="$NameKernel"
					Kernel=
				fi
				Value="${line#*=}"
				if [[ -z "$Kernel" ]]; then
					if [[ -z "$Name" && -n "$Value" ]]; then
						Exports_DisklessMDRoots="$Exports_DisklessMDRoots\n$DisklessMD_Root	$INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(rw,no_root_squash,no_all_squash,sync,no_subtree_check)"
					else
						continue
					fi
				else
					Exports_DisklessMDRoots="$Exports_DisklessMDRoots\n$DisklessMD_Root/$Name	$INTERNAL_SUBNET/$INTERNAL_SUBNET_MASK(rw,no_root_squash,no_all_squash,sync,no_subtree_check)"
				fi
			done
		fi
	fi
done

PopulateSection "/etc/exports" "DisklessMDRoots" "$Exports_DisklessMDRoots"

## Check and start/reload the nfs-kernel-server
if [[ "$(pidof rpc.mountd)" == "" ]] ;then
        service nfs-kernel-server restart
fi
service nfs-kernel-server reload
