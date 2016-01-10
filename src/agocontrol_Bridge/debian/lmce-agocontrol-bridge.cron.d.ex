#
# Regular cron jobs for the lmce-agocontrol-bridge package
#
0 4	* * *	root	[ -x /usr/bin/lmce-agocontrol-bridge_maintenance ] && /usr/bin/lmce-agocontrol-bridge_maintenance
