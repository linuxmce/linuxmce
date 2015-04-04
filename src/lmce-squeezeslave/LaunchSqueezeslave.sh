#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICEDATA_Alsa_Output_Device=74
DEVICEDATA_Pnp_Create_Without_Prompting=164
DEVICETEMPLATE_Squeezebox_Player=58

SQUEEZESLAVE=$(which squeezeslave)
SQUEEZELITE=$(which squeezelite)
[[ -n "$SQUEEZELITE" ]] && SQUEEZESLAVE="$SQUEEZELITE"

#SQUEEZESLAVE=/usr/bin/squeezeslave
SERVER="$DCERouter"
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

echo "Device $DEVICE server $SERVER"
Q="
	SELECT MACaddress
	FROM Device
	WHERE PK_Device='$DEVICE'
"
MAC=$(RunSQL "$Q")

if [[ -z "$MAC" ]] ; then
	# If there is no MAC set in the DB, we'll get it from the system
	MyIF=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $8 }')
	#MyIP=$(/sbin/ifconfig $MyIF | awk 'NR==2 { print substr($2, index($2, ":") + 1) }')
	MAC=$(/sbin/ifconfig $MyIF | awk 'NR==1 { print $5 }')
	#Gateway=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $2 }')
	MAC=$( awk -F: '{ print "00:00:00:"$4":"$5":"$6 }' <<<$MAC )

	# Now write this to the database so user can see it in webadmin
	Q="
		UPDATE Device
		SET MACaddress='$MAC'
		WHERE PK_Device="$DEVICE"
	"
	R=$(RunSQL "$Q")
fi
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

# Announce our presence to the router
# TODO: Pass Room/EntArea?
/usr/pluto/bin/MessageSend "$DCERouter" 0 -1001 2 65 5 "$MAC" 52 3 53 5 49 "$DEVICETEMPLATE_Squeezebox_Player" 55 "$DEVICEDATA_Pnp_Create_Without_Prompting|0"

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
