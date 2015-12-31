#
# Regular cron jobs for the qmediaplayer-plugin package
#
0 4	* * *	root	[ -x /usr/bin/qmediaplayer-plugin_maintenance ] && /usr/bin/qmediaplayer-plugin_maintenance
