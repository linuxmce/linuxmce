#
# Regular cron jobs for the qmediaplayer-player package
#
0 4	* * *	root	[ -x /usr/bin/qmediaplayer-player_maintenance ] && /usr/bin/qmediaplayer-player_maintenance
