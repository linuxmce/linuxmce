#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICEDATA_Alsa_Output_Device=74

SQUEEZESLAVE=$(which squeezeslave)
SQUEEZELITE=$(which squeezelite)
[[ -n "$SQUEEZELITE" ]] && SQUEEZESLAVE="$SQUEEZELITE"

#SQUEEZESLAVE=/usr/bin/squeezeslave
SERVER=dcerouter
DEVICE=0

while getopts ":d:r:" optname
	do
		case "$optname" in
			"d")
				DEVICE=$OPTARG
				;;
			"r")
				SERVER=$OPTARG
				;;
			*)
				;;
		esac
	done

# echo "Device $DEVICE server $SERVER"
# Don't know how to get MAC address without SQL
Q="
	SELECT MACaddress
	FROM Device
	WHERE PK_Device='$DEVICE'
"
MAC=$(RunSQL "$Q")
[[ -z "$MAC" ]] && exit 2

AUDIO_DEVICE=$(GetDeviceData "$DEVICE" "$DEVICEDATA_Alsa_Output_Device")
OUTPUT=""
# echo "Mac $MAC audio device $AUDIO_DEVICE"
if [[ -n "$AUDIO_DEVICE" ]]; then
	if [[ "$SQUEEZESLAVE" = "$SQUEEZELITE" ]]; then
		OUTPUT="$AUDIO_DEVICE"
	else
		OUTPUT=$($SQUEEZESLAVE -L | grep "$AUDIO_DEVICE" | cut -d: -f1)
		#echo "$OUTPUT : $AUDIO_DEVICE"
		# Strip any spaces from the string
		OUTPUT=${OUTPUT//[[:space:]]}
	fi
fi
[[ -z "$OUTPUT" ]] && exit 1

# Build command parameters
PARAMS=""
if [[ "$SQUEEZESLAVE" = "$SQUEEZELITE" ]]; then
	[[ -n "$OUTPUT" ]] && OUTPUT="-o $OUTPUT"
	[[ -n "$SERVER" ]] && SERVER="-s $SERVER"
	[[ -n "$MAC" ]] && MAC="-m $MAC"
else
	[[ -n "$OUTPUT" ]] && OUTPUT="-o$OUTPUT"
	[[ -n "$MAC" ]] && MAC="-m$MAC"
fi
PARAMS="$OUTPUT $MAC $SERVER"


echo "Starting squeezeslave: $SQUEEZESLAVE $PARAMS"
$SQUEEZESLAVE $PARAMS
