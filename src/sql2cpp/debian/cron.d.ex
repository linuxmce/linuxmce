#
# Regular cron jobs for the pluto-sql2cpp package
#
0 4	* * *	root	[ -x /usr/bin/pluto-sql2cpp_maintenance ] && /usr/bin/pluto-sql2cpp_maintenance
