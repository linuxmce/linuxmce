#
# Regular cron jobs for the pluto-vipshared package
#
0 4	* * *	root	[ -x /usr/bin/pluto-vipshared_maintenance ] && /usr/bin/pluto-vipshared_maintenance
