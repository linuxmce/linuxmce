#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICEDATA_Configuration=59
DEVICEDATA_Alsa_Output_Device=74
DEVICEDATA_Pnp_Create_Without_Prompting=164
DEVICETEMPLATE_Squeezebox_Player=58

SQUEEZESLAVE=$(which squeezeslave)
SQUEEZELITE=$(which squeezelite)
[[ -n "$SQUEEZELITE" ]] && SQUEEZESLAVE="$SQUEEZELITE"

#SQUEEZESLAVE=/usr/bin/squeezeslave
SERVER="$DCERouter"
DEVICE="0"

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

[[ "$DEVICE" == "0" ]] && exit 3

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

# Get the description of the device to add it to the commandline for naming the squeezelite

Q="SELECT Description FROM EntertainArea 
	JOIN Device_EntertainArea 
		ON PK_EntertainArea = FK_EntertainArea 
	WHERE FK_Device = $DEVICE;"
BOXNAME=$(RunSQL "$Q")


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

# Check if we already are in the database, and if not, let the router create one
# in our current room.

Q=" SELECT COUNT(*) FROM Device 
	WHERE FK_DeviceTemplate='$DEVICETEMPLATE_Squeezebox_Player'
		AND MACaddress='$MAC';"
INSTALLED=$(RunSQL "$Q")

if [[ "$INSTALLED" == "0" ]]; then
	# We don't have the corresponding Squeezebox Player in our install
	# so let's create it. 
	# We need the room we are in, so we don't bother the user with a question
	
	Q="SELECT FK_Room FROM Device WHERE PK_Device = $DEVICE"
	ROOM=$(RunSQL "$Q")

	if [[ "x$ROOM" == "x" ]]; then
		# we have no room, so we don't add the information
		echo ""
	else
		# we add the room parameter to the message send.
		ROOM="57 $ROOM"
	fi	

	/usr/pluto/bin/MessageSend "$DCERouter" 0 4 1 718 44 "$DEVICETEMPLATE_Squeezebox_Player" 47 "$MAC" $ROOM
fi
	

# Build command parameters
PARAMS=""
if [[ "$SQUEEZESLAVE" = "$SQUEEZELITE" ]]; then
	[[ -n "$OUTPUT" ]] && OUTPUT="-o $OUTPUT"
	[[ -n "$SERVER" ]] && SERVER="-s $SERVER"
	[[ -n "$MAC" ]] && MAC="-m $MAC"
	[[ -n "$BOXNAME" ]] && OUTPUT="$OUTPUT -n '$BOXNAME'"
	CONFIGURATION=$(GetDeviceData "$DEVICE" "$DEVICEDATA_Configuration")
else
	[[ -n "$OUTPUT" ]] && OUTPUT="-o$OUTPUT"
	[[ -n "$MAC" ]] && MAC="-m$MAC"
fi
PARAMS="$OUTPUT $MAC $SERVER $CONFIGURATION"

echo "Starting squeezeslave: $SQUEEZESLAVE $PARAMS"
$SQUEEZESLAVE $PARAMS
exit $?
