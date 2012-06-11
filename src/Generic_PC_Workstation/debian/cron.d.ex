#
# Regular cron jobs for the lmce-generic-pc-workstation package
#
0 4	* * *	root	[ -x /usr/bin/lmce-generic-pc-workstation_maintenance ] && /usr/bin/lmce-generic-pc-workstation_maintenance
