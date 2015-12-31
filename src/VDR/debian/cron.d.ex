#
# Regular cron jobs for the pluto-vdr package
#
0 4	* * *	root	[ -x /usr/bin/pluto-vdr_maintenance ] && /usr/bin/pluto-vdr_maintenance
