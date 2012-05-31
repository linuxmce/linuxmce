#
# Regular cron jobs for the pluto-system-database package
#
0 4	* * *	root	[ -x /usr/bin/pluto-system-database_maintenance ] && /usr/bin/pluto-system-database_maintenance
