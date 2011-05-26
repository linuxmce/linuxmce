#!/bin/bash
#
# This is called from VDR_Player when the user selects TV.
# 
# On the core, we just start vdr-sxfe and close it in the end.
# On the MDs, we start VDR and vdr-sxfe after 5 seconds. The 5 seconds are needed, as 
# it takes VDR a little bit to get the output device ready.
#
. /usr/pluto/bin/Config_Ops.sh

ConfEval

if [[ "$PK_Device" -eq "1" ]]; then
	# We are running on the core, and do not want to stop VDR
	trap 'killall -KILL vdr-sxfe' EXIT
	# Make sure VDR is running
	invoke-rc.d vdr start
else
	# We are running on a MD, and don't want VDR running after the user closes the TV app.
	trap 'killall -KILL vdr-sxfe ; invoke-rc.d vdr stop' EXIT
	invoke-rc.d vdr restart

	if  [ -x /usr/bin/nmap ] ; then
		VDRRUNNING=0
		while [[ "$VDRRUNNING" = "0" ]]; do
			nmap 127.0.0.1 -p 22 --open | grep open -iq && VDRRUNNING=1
			sleep 1
		done
	else
		sleep 5
	fi
fi
/usr/bin/vdr-sxfe --reconnect xvdr://127.0.0.1 --post tvtime:method=use_vo_driver --tcp --syslog --verbose
