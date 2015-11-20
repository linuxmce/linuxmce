#!/bin/bash
#
# This is called from VDR_Player when the user selects TV.
# 
# On the core, we just start vdr-sxfe and close it in the end.
# On the MDs, we start VDR. If nmap is installed, we use it
# to check for a running VDR. As soon as it runs, vdr-sxfe starts.
#
# When StartVDRClient ends, vdr-sxfe is killed, and vdr on the moon
# is restarted
. /usr/pluto/bin/Config_Ops.sh

ConfEval

LOCKFILE="/usr/pluto/locks/VDRrunning"
XINE_IP=$1
if [[ "$PK_Device" -eq "1" ]]; then
	# We are running on the core, and do not want to stop VDR
	trap 'killall -KILL vdr-sxfe' EXIT
	# Make sure VDR is running
	service vdr restart
else
	# We are running on a MD, and don't want VDR running after the user closes the TV app.
	trap 'killall -KILL vdr-sxfe ; rm -f $LOCKFILE ;  service vdr restart' EXIT
	service vdr restart

fi
if  [ -x /usr/bin/nmap ] ; then
	VDRRUNNING=0
	while [[ "$VDRRUNNING" = "0" ]]; do
		nmap 127.0.0.1 -p 22 --open | grep open -iq && VDRRUNNING=1
		sleep 1
	done
else
	sleep 5
fi
svdrpsend -p 2001 HITK Ok
touch $LOCKFILE
ERRORLEVEL=0
while [ "$ERRORLEVEL" -ne "1" ] ; do
	/usr/bin/vdr-sxfe $XINE_IP --reconnect --post tvtime:method=use_vo_driver --fullscreen --tcp --syslog --verbose
	ERRORLEVEL=$?
done
