#!/bin/bash

############################################################
##
## Detect USB Game pads to be managed by USB_Game_Pad
##
## Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
##
## Synopsis: 
##    This tool is actually very simple. It simply:
##
##    (1) Checks for a running USB_Game_Pad process...
##    (2) If no, check for a device node /dev/input/js0
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
    if [[ -n "$(pgrep USBGP_up.sh)" ]]; then
    # USB_Game_Pad is running, don't do anything.
	echo "USB Game Pad already running."
    else
    # USB Game Pad isn't running. Let's check for any joystick.
	if [[ -c "/dev/input/js0" ]]; then
	# USB Joystick detected, add USB game pad.
	    PK_Device_MD=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Generic_PC_as_MD" norecursion)
	    /usr/pluto/bin/MessageSend "$DCERouter" $PK_Device -1001 2 65 52 4 53 2 49 2189
	    /usr/pluto/bin/USBGP_up.sh &
	else
	    echo "No USB Game Pads detected."
	# USB Joystick was not detected.
	fi
    fi
    
    ## Wait another 5 seconds before trying again.
    sleep 5

done
