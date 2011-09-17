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

if [[ -f /dev/input/js0 ]] ;
then
    # Joystick(s) detected.
    InstallPackage
    echo "/usr/pluto/bin/USB_Game_Pad -l /var/log/pluto/avremote.log -r $DCERouter -d -1003 -H localhost -P $AVWizard_Port"
else
    # No joysticks detected.
    echo ""
fi
