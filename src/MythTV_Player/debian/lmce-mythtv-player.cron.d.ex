#
# Regular cron jobs for the lmce-mythtv-player package
#
0 4	* * *	root	[ -x /usr/bin/lmce-mythtv-player_maintenance ] && /usr/bin/lmce-mythtv-player_maintenance
