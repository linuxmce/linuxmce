#
# Regular cron jobs for the pluto-media package
#
0 4	* * *	root	[ -x /usr/bin/pluto-media_maintenance ] && /usr/bin/pluto-media_maintenance
