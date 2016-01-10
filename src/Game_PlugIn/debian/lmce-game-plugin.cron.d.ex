#
# Regular cron jobs for the lmce-game-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-game-plugin_maintenance ] && /usr/bin/lmce-game-plugin_maintenance
