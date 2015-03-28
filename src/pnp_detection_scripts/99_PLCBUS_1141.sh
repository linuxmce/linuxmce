#!/bin/bash
. /usr/pluto/bin/SQL_Ops.sh

Device="$1"
PNPQueue="$2"
ScriptName="$4"
PLCBUSTemplate=1920

NotPLCBUS()
{
	echo "It's not a PLCBUS interface"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $Device 1 806 224 $PNPQueue 13 "$ScriptName" 44 0
	exit 0
}

ParmsData=$(RunSQL "SELECT Parms FROM PnpQueue WHERE PK_PnpQueue=$PNPQueue")
if [[ -z "$ParmsData" ]]; then
	NotPLCBUS
fi

SerialLocation=$(echo "$ParmsData" | cut -d'|' -f2)
SerialPort=TranslateSerialPort "$SerialLocation"
if [[ ! -c "$SerialPort" ]]; then
	NotPLCBUS
fi

echo "PLCBUS Detection script queue $PNPQueue, serial port $SerialPort"
/usr/pluto/bin/TestSerialPort -p $SerialPort -P N81 -b 9600 -t "\02\05\00\00\22\00\00\03" -i 3 -s "\02\06\00\00\22"
if [[ "$?" -ne 0 ]]; then
	NotPLCBUS
else
	echo "It is a PLCBUS interface"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $Device 1 806 224 $PNPQueue 13 "$ScriptName" 44 $PLCBUSTemplate
fi
