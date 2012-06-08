#
# Regular cron jobs for the pluto-xine-player package
#
0 4	* * *	root	[ -x /usr/bin/pluto-xine-player_maintenance ] && /usr/bin/pluto-xine-player_maintenance
