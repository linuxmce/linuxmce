#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICETEMPLATE_HAL=1808
DEVICETEMPLATE_AppServer=26

#lshwd is obsolete and unmaintained in 5 years.  removing to see if anything breaks - phenigma - 2014-04-09
#lshwd # needed for some systems which won't report usb devices until you run lshwd first (bug in usb architecture)

HALdev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_HAL" "norecursion")
if [[ -z "$HALdev" ]]; then
	# HAL device not found
	exit 1
fi

screen -d -m -S "HAL-$HALdev" /usr/pluto/bin/Spawn_Device.sh $HALdev $DCERouter HAL

ASdev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_AppServer" "norecursion")
if [[ -z "$ASdev" ]]; then
	# App Server device not found
	exit 1
fi

screen -d -m -S "APP-$ASdev" /usr/pluto/bin/Spawn_Device.sh $ASdev $DCERouter App_Server
