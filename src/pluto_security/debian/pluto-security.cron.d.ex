#
# Regular cron jobs for the pluto-security package
#
0 4	* * *	root	[ -x /usr/bin/pluto-security_maintenance ] && /usr/bin/pluto-security_maintenance
