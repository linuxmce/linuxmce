#
# Regular cron jobs for the pluto-sqlcvs package
#
0 4	* * *	root	[ -x /usr/bin/pluto-sqlcvs_maintenance ] && /usr/bin/pluto-sqlcvs_maintenance
