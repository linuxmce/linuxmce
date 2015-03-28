#!/bin/bash
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

Device="$1"
PNPQueue="$2"
ScriptName="$4"
ZWaveTemplate=1754

NotZWave()
{
	echo "It's not a ZWave interface"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $Device 1 806 224 $PNPQueue 13 "$ScriptName" 44 0
	exit 0
}

ParmsData=$(RunSQL "SELECT Parms FROM PnpQueue WHERE PK_PnpQueue=$PNPQueue")
if [[ -z "$ParmsData" ]]; then
	NotZWave
fi

SerialLocation=$(echo "$ParmsData" | cut -d'|' -f2)
SerialPort=TranslateSerialPort "$SerialLocation"
if [[ ! -c "$SerialPort" ]]; then
	NotZWave
fi

echo "ZWave Detection script queue $PNPQueue, serial port $SerialPort"
/usr/pluto/bin/TestSerialPort -p $SerialPort -P N81 -b 115200 -t "\01\03\00\15\e9" -i 1 -s "Z-Wave"
if [[ "$?" -ne 0 ]]; then
	NotZWave
else
	echo "It is a ZWave interface"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $Device 1 806 224 $PNPQueue 13 "$ScriptName" 44 $ZWaveTemplate
fi
