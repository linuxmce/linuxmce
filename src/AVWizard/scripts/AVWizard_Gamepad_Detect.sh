#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/AVWizard-Common.sh

Log()
{
	echo "$1" >>/var/log/pluto/avremote_detect.log
}

InstallPackage()
{
	if ! PackageIsInstalled "lmce-usb-gamepad"; then
		Log "Installing package: 'lmce-usb-gamepad'"
		apt-get -y install lmce-usb-gamepad
	else
		Log "Package is installed: 'lmce-usb-gamepad'"
	fi
}

DEVICES=/dev/input/js?
shopt -s nullglob
for DEV in $DEVICES
do
	if [[ -c "$DEV" ]]; then
		# Check that it is actually a joystick and not a mouse or accelerometer
		if [[ `udevadm info --query property --name $DEV | grep 'ID_INPUT_JOYSTICK=1'` ]]; then
			# USB Joystick detected, add USB game pad.
			InstallPackage
			echo "/usr/pluto/bin/USB_Game_Pad -l /var/log/pluto/avremote.log -r $DCERouter -d -1003 -H localhost -P $AVWizard_Port"
			break
		else
			# USB Joystick was not detected.
			echo ""
		fi
	    else
		# USB Joystick was not detected.
		echo ""
	fi
done
