#
# Regular cron jobs for the pluto-mplayer-player package
#
0 4	* * *	root	[ -x /usr/bin/pluto-mplayer-player_maintenance ] && /usr/bin/pluto-mplayer-player_maintenance
