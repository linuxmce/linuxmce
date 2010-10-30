#!/bin/bash
. /usr/pluto/bin/SQL_Ops.sh

HALDevice="$1"
PNPQueue="$2"
ScriptName="$4"
ZWaveTemplate=1754

NotZWave()
{
	echo "It's not a ZWave interface"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $HALDevice 1 806 224 $PNPQueue 13 "$ScriptName" 44 0
	exit 0
}

SerialNumber=$(RunSQL "SELECT SerialNumber FROM PnpQueue WHERE PK_PnpQueue=$PNPQueue")
if [[ -z "$SerialNumber" ]]; then
	NotZWave
fi

SysfsPath=$(hal-get-property --udi "$SerialNumber" --key "usb_device.linux.sysfs_path")
if [[ -z "$SysfsPath" ]]; then
	NotZWave
fi

SysfsDev=$(find "$SysfsPath" | egrep '/tty[:/]' | head -1)
if [[ -z "$SysfsDev" ]]; then
	NotZWave
fi

DevBase=$(basename "$SysfsDev")
SerialPort=/dev/$DevBase
if [[ ! -c "$SerialPort" ]]; then
	NotZWave
fi

echo "ZWave Detection script queue $PNPQueue, serial port $SerialPort"
/usr/pluto/bin/TestSerialPort -p $SerialPort -P N81 -b 115200 -t "\01\03\00\15\e9" -i 1 -s "Z-Wave"
if [[ "$?" -ne 0 ]]; then
	NotZWave
else
	echo "It is a ZWave interface"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $HALDevice 1 806 224 $PNPQueue 13 "$ScriptName" 44 $ZWaveTemplate
fi
