#!/bin/bash

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

str_DEV=";<pl_dev_$Device_ID>
type = 7970
description = SEP$Device_MAC
autologin = $Device_EXT
transfer = off
park = off
speeddial = 
cfwdall = off
cfwdbusy = off
dtmfmode = inband
device => SEP$Device_MAC
;</pl_dev_$Device_ID>
"

str_LINE=";<pl_line_$Device_ID>
id = $Device_EXT
pin = 
label = $Device_EXT
description = $Device_EXT
context = from-internal
incominglimit = 2
transfer = off
secondary_dialtone_digits = 9
secondary_dialtone_tone = 0x22
cid_name = pl_$Device_ID
cid_num = $Device_EXT
line => $Device_EXT
;</pl_line_$Device_ID>
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

str_DEV=";<pl_dev_$Device_ID>
type = 7910
description = SEP$Device_MAC
autologin = $Device_EXT
transfer = off
park = off
speeddial = 
cfwdall = off
cfwdbusy = off
dtmfmode = inband
device => SEP$Device_MAC
;</pl_dev_$Device_ID>
"

str_LINE=";<pl_line_$Device_ID>
id = $Device_EXT
pin = 
label = $Device_EXT
description = $Device_EXT
context = from-internal
incominglimit = 2
transfer = off
secondary_dialtone_digits = 9
secondary_dialtone_tone = 0x22
cid_name = pl_$Device_ID
cid_num = $Device_EXT
line => $Device_EXT
;</pl_line_$Device_ID>
"

	devices="$devices$str_DEV"
	lines="$lines$str_LINE"
done

while read line; do
	line="${line#+}"
	line="${line%+}"

	echo "$line"
	if [[ "$line" == "[devices]" ]]; then
		echo "$devices"
	elif [[ "$line" == "[lines]" ]]; then
		echo "$lines"
	fi
done < <(sed 's/^/+/; s/$/+/' "$SCCPconfFile".template) >"$SCCPconfFile"
`chown asterisk:asterisk /etc/asterisk/*`

