#
# Regular cron jobs for the pluto-irbase package
#
0 4	* * *	root	[ -x /usr/bin/pluto-irbase_maintenance ] && /usr/bin/pluto-irbase_maintenance
