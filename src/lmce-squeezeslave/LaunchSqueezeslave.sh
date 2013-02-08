#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICEDATA_Alsa_Output_Device=74

SQUEEZESLAVE=/usr/bin/squeezeslave
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
AUDIO_DEVICE=$(GetDeviceData "$DEVICE" "$DEVICEDATA_Alsa_Output_Device")

OUTPUT=""
# echo "Mac $MAC audio device $AUDIO_DEVICE"
if [ -n "$AUDIO_DEVICE" ]; then
    OUTPUT=$($SQUEEZESLAVE -L | grep "$AUDIO_DEVICE" | cut -d: -f1)
    #echo "$OUTPUT : $AUDIO_DEVICE"
    # Strip any spaces from the string
    OUTPUT=${OUTPUT//[[:space:]]}
fi

# Build command parameters
PARAMS="";
if [ -n "$OUTPUT" ]; then
    PARAMS="$PARAMS -o$OUTPUT"
fi
if [ -n "$MAC" ]; then
    PARAMS="$PARAMS -m$MAC"
fi
if [ -n "$SERVER" ]; then
    PARAMS="$PARAMS $SERVER"
fi
echo "Starting squeezeslave: $SQUEEZESLAVE $PARAMS"
$SQUEEZESLAVE $PARAMS
