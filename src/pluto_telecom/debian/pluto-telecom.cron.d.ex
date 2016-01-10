#
# Regular cron jobs for the pluto-telecom package
#
0 4	* * *	root	[ -x /usr/bin/pluto-telecom_maintenance ] && /usr/bin/pluto-telecom_maintenance
