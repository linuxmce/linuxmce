#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/CaptureCards_Utils.sh

DEVICETEMPLATE_WinTV_PVR_USB_2=1874
DEVICETEMPLATE_GeneralInfoPlugin=27
DEVICEDATA_Location_on_PCI_bus=175
DEVICEDATA_Setup_Script=189

CaptureCard_PCISlot()
{
	local PK_Device_CC="$1"
	local Q Slot

	if [[ -z "$PK_Device_CC" ]]; then
		return 1
	fi

	Q="
		SELECT IK_DeviceData
		FROM Device_DeviceData
		WHERE FK_Device='$PK_Device_CC' AND FK_DeviceData='$DEVICEDATA_Location_on_PCI_bus'
	"
	Slot="$(RunSQL "$Q")"
	Slot="${Slot##*/}"
	Slot="${Slot#*:}"

	echo "$Slot"
	return 0
}

OurDevice()
{
	local Device="$1" Slot="$2" ID="$3"
	local Q R Template

	if [[ -z "$Slot" || -z "$ID" ]]; then
		return 1
	fi

	Q="
		SELECT COUNT(VendorModelID)
		FROM DHCPDevice
		JOIN Device ON Device.FK_DeviceTemplate=DHCPDevice.FK_DeviceTemplate
		WHERE
			PK_Device='$Device'
			AND '$ID' LIKE CONCAT(VendorModelID,'%')
	"
	R=$(RunSQL "$Q")

	[[ "$R" != 0 ]]
	return $?
}

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
	local Q CmdLine

	if [[ -z "$Device" ]]; then
		return 1
	fi

	# COMMAND: #791 - Set Enable Status
	GeneralInfoPlugin_DeviceID=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_GeneralInfoPlugin" "" "")
	/usr/pluto/bin/MessageSend "$DCERouter" 0 $GeneralInfoPlugin_DeviceID 1 791 2 $Device 208 1

	if [[ "$Slot" != "" ]] ;then
		Q="
			SELECT DDDD.IK_DeviceData
			FROM DeviceTemplate_DeviceData DDDD
			JOIN Device D ON D.FK_DeviceTemplate=DDDD.FK_DeviceTemplate
				AND D.PK_Device='$Device'
				AND DDDD.FK_DeviceData='$DEVICEDATA_Setup_Script'
		"
		CmdLine=$(RunSQL "$Q")
	
		if [[ -n "$CmdLine" ]]; then
			/usr/pluto/bin/"$CmdLine" "$Device" "$Slot"
			# card script will also update ports
		else
			UpdatePorts "$Device" "$Slot"
		fi
	fi
}


CardDevice_Template=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = $1")
echo "$(date -R) Giving the card 5 seconds to initialize so the /dev/videoX entry is there"
sleep 5
echo "--> Setting up card '$1'"

# PVRUSB2 capture card
CardDevice_PciID=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $1 AND FK_DeviceData = $DEVICEDATA_Location_on_PCI_bus")
if [[ -z "$CardDevice_PciID" ]];then
	exit
fi

CardDevice_HalUDI=$(udevadm info --query=property --path "/sys/devices/$CardDevice_PciID")
if [[ -z "$CardDevice_HalUDI" ]] ;then
	ls -l /dev/video*
	DisableDevice "$1"
	exit
fi

CardDevice_UsbSerial=$(udevadm info --query=property --path "/sys/devices/$CardDevice_PciID" | grep ID_SERIAL | cut -d'=' -f2)
if [[ -z "$CardDevice_UsbSerial" ]] ;then
	DisableDevice "$1"
	exit
fi

PK_Country=$(RunSQL "SELECT FK_Country FROM Installation WHERE PK_Installation=$PK_Installation")

Port=""
for dir in /sys/class/pvrusb2/* ;do
	echo "Checking directory $dir"
	if [[ "$(cat $dir/device/serial)" == "$CardDevice_UsbSerial" ]]; then
		Port="video$(cat $dir/v4l_minor_number)"
		
		if [[ "$PK_Country" = "840" ]] ;then  # U.S.
			echo NTSC-M >$dir/ctl_video_standard/cur_val
		fi
	fi
done

if [[ "$Port" != "" ]] ;then
	echo "Enabling $1 with $Port"
	Slot=""
	EnableDevice "$1"
	UpdatePorts_NoFind "$1" "$Port"
else
	echo "Disabling $1"
	DisableDevice "$1"
fi	
