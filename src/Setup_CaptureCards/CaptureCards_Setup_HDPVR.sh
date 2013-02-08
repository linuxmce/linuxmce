#!/bin/bash
. /usr/pluto/bin/CaptureCards_Utils.sh

DEVICETEMPLATE_GeneralInfoPlugin=27
DEVICEDATA_Location_on_PCI_bus=175


DisableDevice()
{

	local Device="$1"
	local Q

	if [[ -z "$Device" ]]; then
		return 1
	fi
	
	# COMMAND: #791 - Set Enable Status
	GeneralInfoPlugin_DeviceID=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_GeneralInfoPlugin" "" "")
	/usr/pluto/bin/MessageSend "$DCERouter" 0 $GeneralInfoPlugin_DeviceID 1 791 2 $Device 208 0

}

EnableDevice()
{
	local Device="$1"
	local Block="$2"
	local Q CmdLine

	if [[ -z "$Device" ]]; then
		return 1
	fi

	# COMMAND: #791 - Set Enable Status
	GeneralInfoPlugin_DeviceID=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_GeneralInfoPlugin" "" "")
	/usr/pluto/bin/MessageSend "$DCERouter" 0 $GeneralInfoPlugin_DeviceID 1 791 2 $Device 208 1

	UpdatePorts_NoFind "$Device" "$Block"
}


CardDevice_Template=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = $1")
echo "$(date -R) Giving the card 5 seconds to initialize so the /dev/videoX entry is there"
sleep 5
echo "--> Setting up card '$1'"

#HDPVR  capture card
CardDevice_PciID=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $1 AND FK_DeviceData = $DEVICEDATA_Location_on_PCI_bus")
if [[ -z "$CardDevice_PciID" ]];then
	DisableDevice $1
	exit
fi
dir="/sys/devices/$CardDevice_PciID"
dev=$(ls $dir/video4linux/)
if [[ -z "$dev" ]];then
	DisableDevice $1
	exit
fi
EnableDevice $1 $dev
