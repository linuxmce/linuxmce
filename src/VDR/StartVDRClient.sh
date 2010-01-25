#!/bin/bash
#
# This is called from VDR_Player when the user selects TV.
# 
# On the core, we just start vdr-sxfe and close it in the end.
# On the MDs, we start VDR and vdr-sxfe after 5 seconds. The 5 seconds are needed, as 
# it takes VDR a little bit to get the output device ready.
#
. /usr/pluto/bin/Config_Ops.sh

ConfEval()

if [[ "$PK_Device" -eq "1" ]]; then
	# We are running on the core, and do not want to stop VDR
	trap 'killall -KILL vdr-sxfe' EXIT
else
	# We are running on a MD, and don't want VDR running after the user closes the TV app.
	trap 'killall -KILL vdr-sxfe && invoke-rc.de vdr stop'
	invoke-rc.d vdr restart
	sleep 5
fi

/usr/bin/vdr-sxfe --reconnect xvdr://127.0.0.1 --post tvtime:method=use_vo_driver --tcp --syslog --verbose
