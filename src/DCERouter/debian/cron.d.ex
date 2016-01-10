#
# Regular cron jobs for the pluto-dcerouter package
#
0 4	* * *	root	[ -x /usr/bin/pluto-dcerouter_maintenance ] && /usr/bin/pluto-dcerouter_maintenance
