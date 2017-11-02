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
TPL_ZFS_POOL=2351
TPL_ZFS_MIRROR=2352
TPL_ZFS_RAIDZ1=2353
TPL_ZFS_RAIDZ2=2354
TPL_ZFS_RAIDZ3=2355

## Lookup our internal storage devices in the db
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate IN ($TPL_GENERIC_INTERNAL_DRIVE,$TPL_RAID_0,$TPL_RAID_1,$TPL_RAID_5,$TPL_ZFS_POOL,$TPL_ZFS_MIRROR,$TPL_ZFS_RAIDZ1,$TPL_ZFS_RAIDZ2,$TPL_ZFS_RAIDZ3) AND FK_Device_ControlledVia=$PK_Device"
InternalOwnStorageDevices=$(RunSQL "$Q")

for Device in $InternalOwnStorageDevices; do
	Device_ID=$(Field 1 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"

	Exports_InternalStorageDevices="$Exports_InternalStorageDevices\n
[Storage$Device_ID\$]
comment = Storage Device $Device_ID
browseable = yes
writable = yes
create mask = 0770
directory mask = 0770
path = /mnt/device/$Device_ID
public = no
guest ok = no
force user = root
force group = root	
"
done

if ! BlacklistConfFiles '/etc/samba/smb.conf' ;then
	if [ ! -e /etc/samba/smb.conf.pbackup ] ;then
		cp /etc/samba/smb.conf /etc/samba/smb.conf.pbackup
	fi
	PopulateSection "/etc/samba/smb.conf" "InternalStorageDevices" "$Exports_InternalStorageDevices"
fi

## Run SetupUsers_Homes so it readds the sambahelper user to smbpasswd file
/usr/pluto/bin/SetupUsers_Homes.sh

## Check and start/reload the nfs-kernel-server
if [[ "$(pidof smbd)" == "" ]] ;then
	service smbd restart
else
	service smbd reload
fi
