#
# Regular cron jobs for the pluto-marbella-database package
#
0 4	* * *	root	[ -x /usr/bin/pluto-marbella-database_maintenance ] && /usr/bin/pluto-marbella-database_maintenance
