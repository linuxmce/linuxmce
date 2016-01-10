#
# Regular cron jobs for the pluto-pluto-media-db package
#
0 4	* * *	root	[ -x /usr/bin/pluto-pluto-media-db_maintenance ] && /usr/bin/pluto-pluto-media-db_maintenance
