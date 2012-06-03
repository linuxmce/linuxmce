#
# Regular cron jobs for the pluto-proxy-orbiter package
#
0 4	* * *	root	[ -x /usr/bin/pluto-proxy-orbiter_maintenance ] && /usr/bin/pluto-proxy-orbiter_maintenance
