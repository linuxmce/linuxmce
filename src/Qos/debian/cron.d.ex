#
# Regular cron jobs for the pluto-qos package
#
0 4	* * *	root	[ -x /usr/bin/pluto-qos_maintenance ] && /usr/bin/pluto-qos_maintenance
