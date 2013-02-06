#!/bin/bash

############################################################
##
## Detect USB Game pads to be managed by USB_Game_Pad
##
## Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
## Multi-device detection added by: phenigma <phenigma@hotmail.com>
##
## Synopsis:
##    This tool is actually very simple. It simply:
##
##    (1) Checks for a running USB_Game_Pad process...
##    (2) If no, check for a device node /dev/input/js?
##    (3) If yes, send a device detected event with a DT 
##        of 2189, the USB Game Pad.
##
############################################################

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/pluto.func

DEVICETEMPLATE_Generic_PC_as_MD="28"

if [[ $1 != "background" ]] ;then
	echo "Backgrounding ..."
	screen -d -m -S "GamepadRadar" "$0" "background"
	exit 0
fi

while [ 1 ]
do
	# Check for running USB Game Pad process.
	if [[ -n "$(pgrep USB_Game_Pad)" ]]; then
		# USB_Game_Pad is running, don't do anything.
		echo "USB Game Pad already running."
	else
		# USB Game Pad isn't running. Let's check for joysticks.
		DEVICES=/dev/input/js?

		# if no $DEVICES exist, return nothing in $DEV rather than copying $DEVICES to $DEV
		shopt -s nullglob 

		for DEV in $DEVICES
		do
			if [[ -c "$DEV" ]]; then
			# Check that it is actually a joystick and not a mouse or accelerometer
				if [[ `udevadm info --query property --name $DEV | grep 'ID_INPUT_JOYSTICK=1'` ]]; then
					echo "USB Joystick detected adding USB_Game_Pad device $DEV."
#					PK_Device_MD=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Generic_PC_as_MD" norecursion)
					while [[ -z "$(pgrep USB_Game_Pad)" ]]; do
						echo "Sending message to dcerouter from $PK_Device"
						/usr/pluto/bin/MessageSend "$DCERouter" $PK_Device -1001 2 65 52 4 53 2 49 2189
						sleep 30
					done
					break
				else
					# USB Joystick was not detected.
					echo "Non-Joystick USB Game Pad detected on $DEV - not adding device."
				fi
			else
				# USB Joystick was not detected.
				echo "No USB Game Pads detected - no /dev/input/js? devices exist."
			fi
		done
	fi

	## Wait another 5 seconds before trying again.
	sleep 5
done
