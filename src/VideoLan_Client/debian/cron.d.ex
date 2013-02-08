#
# Regular cron jobs for the pluto-videolan-client package
#
0 4	* * *	root	[ -x /usr/bin/pluto-videolan-client_maintenance ] && /usr/bin/pluto-videolan-client_maintenance
