#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

/usr/pluto/bin/UpdateAvailableSoundCards.sh

DEVICETEMPLATE_SqueezeBox_Slave=1964
DEVICEDATA_Sound_card=284

Q="SELECT PK_Device,MACaddress FROM Device WHERE FK_Device_ControlledVia=$PK_Device AND FK_DeviceTemplate=$DEVICETEMPLATE_SqueezeBox_Slave"
R=$(RunSQL "$Q")

if [[ -z "$R" ]]; then
	exit 0
fi

Cards=()
while read Number Device; do
	Cards=("${Cards[@]}" "$Device=$Number")
done < <(squeezeslave -L 2>/dev/null | egrep 'hw:|Virtual_' | sed -r '/hw:/ s,[^0-9]*([0-9]+):.*\((hw:.*)\),\1 \2,g; /Virtual_/ s,[^0-9]*([0-9]+):.*(Virtual_[0-9]+),\1 \2,g')

for Row in $R; do
	Dev=$(Field 1 "$Row")
	Mac=$(Field 2 "$Row")
	SoundCard=$(GetDeviceData "$Dev" "$DEVICEDATA_Sound_card")
	SoundCard=$(TranslateSoundCard "$SoundCard")

	if [[ -z "$SoundCard" ]]; then
		continue
	fi

	if [[ "$SoundCard" != "Virtual_"* ]]; then
		for Card in "${Cards[@]}"; do
			if [[ "$Card" == hw:* && "$Card" == "hw:$SoundCard,0"* ]]; then
				SqueezeCard="-o${Card#*=}"
			fi
		done
	else
		SqueezeCard="-n$SoundCard"
	fi

	if [[ -z "$SqueezeCard" ]]; then
		continue
	fi

	squeezeslave "$SqueezeCard" dcerouter -R -m"$Mac" -M/var/log/squeezeslave_"$Dev".log 2>/dev/null
	sleep 1 ## Wait 1s before starting the next squeezeslave, or it might fail to start
done
