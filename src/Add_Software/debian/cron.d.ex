#
# Regular cron jobs for the pluto-add-software package
#
0 4	* * *	root	[ -x /usr/bin/pluto-add-software_maintenance ] && /usr/bin/pluto-add-software_maintenance
