#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# Usage:
# HDHomeRunScan.sh <Tuner_ID> <HDHR_ID>
# where Tuner_ID is the PK of the actual tuner device
# where HDHR_ID is the PK of the HDHomerun device
PK_Tuner=$1
PK_HDHR=$2



Q="SELECT IPAddress
        FROM Device
        WHERE PK_Device='$PK_HDHR'"
HDHR_IP=$(RunSQL "$Q") # Can be either the IP address of the HDHR, or the HDHR ID number. We'll use the IP

Q="SELECT IK_DeviceData
	FROM Device_DeviceData
	WHERE FK_Device='$PK_Tuner' AND FK_DeviceData=12 LIMIT 1"
Port=$(RunSQL "$Q")

echo "$(date -R) HDHomeRunScan Starting /usr/pluto/bin/hdhomerun_mythconfig $PK_Tuner $HDHR_IP $Port"

/usr/pluto/bin/hdhomerun_mythconfig "$PK_Tuner" "$HDHR_IP" "$Port" "$MySqlHost" root "" mythconverg

if [[ "$?" == "0" ]] ;then
	sleep 2  # Give the database a chance to flush???  Shouldn't be necessary, but the modulation value somehow isn't getting written out
	/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 $PK_Tuner 20 1
else
	/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 $PK_Tuner 20 -1
fi

echo "$(date -R) HDHomeRunScan done"
