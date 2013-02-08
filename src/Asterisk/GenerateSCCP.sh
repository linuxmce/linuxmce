#!/bin/bash

#
# This file is no longer needed. Everything is done in db_phone_config.sh
#
exit 0


. /usr/pluto/bin/SQL_Ops.sh

UseDB asterisk

Q="
	SELECT description
	FROM devices
"
R=$(RunSQL "$Q")
R="${R//pl_}"

/usr/pluto/bin/sync_pluto2amp.pl $R

SCCPconfFile="/etc/asterisk/sccp.conf"
if [[ ! -f "$SCCPconfFile".template ]]; then
	cp "$SCCPconfFile"{,.template}
fi

UseDB $MySqlDBName

DEVICEDATA_PhoneNumber=31
DEVICETEMPLATE_Cisco_7970=1726
DEVICETEMPLATE_Cisco_7910=1968

Q="
	SELECT PK_Device,MACAddress,IK_DeviceData
	FROM Device
	JOIN Device_DeviceData ON PK_Device=FK_Device
	WHERE
		FK_DeviceData=$DEVICEDATA_PhoneNumber
		AND FK_DeviceTemplate=$DEVICETEMPLATE_Cisco_7970
"
R=$(RunSQL "$Q")

for Row in $R; do
	Device_ID=$(Field 1 "$Row")
	Device_MAC=$(Field 2 "$Row")
	Device_EXT=$(Field 3 "$Row")

	Device_MAC="${Device_MAC//:}"
	Device_MAC=$(echo "$Device_MAC" | tr '[a-z]' '[A-Z]')

str_DEV="[SEP$Device_MAC](7970)
description = LinuxMCE ext $Device_EXT
button = line, $Device_EXT
"

str_LINE="[$Device_EXT](defaultline)
id = $Device_EXT
pin = 
label = $Device_EXT
description = $Device_EXT
cid_name = pl_$Device_ID
cid_num = $Device_EXT
"

	devices="$devices$str_DEV"
	lines="$lines$str_LINE"
done


Q="
	SELECT PK_Device,MACAddress,IK_DeviceData
	FROM Device
	JOIN Device_DeviceData ON PK_Device=FK_Device
	WHERE
		FK_DeviceData=$DEVICEDATA_PhoneNumber
		AND FK_DeviceTemplate=$DEVICETEMPLATE_Cisco_7910
"
R=$(RunSQL "$Q")

for Row in $R; do
	Device_ID=$(Field 1 "$Row")
	Device_MAC=$(Field 2 "$Row")
	Device_EXT=$(Field 3 "$Row")

	Device_MAC="${Device_MAC//:}"
	Device_MAC=$(echo "$Device_MAC" | tr '[a-z]' '[A-Z]')

str_DEV="[SEP$Device_MAC](7910)
description = LinuxMCE ext $Device_EXT
button = line, $Device_EXT
"

str_LINE="[$Device_EXT](defaultline)
id = $Device_EXT
pin = 
label = $Device_EXT
description = $Device_EXT
cid_name = pl_$Device_ID
cid_num = $Device_EXT
"

	devices="$devices$str_DEV"
	lines="$lines$str_LINE"
done

cp $SCCPconfFile.template $SCCPconfFile

echo "$devices" >> $SCCPconfFile
echo "$lines" >> $SCCPconfFile

chown asterisk:asterisk /etc/asterisk/*

