#
# Regular cron jobs for the lmce-game package
#
0 4	* * *	root	[ -x /usr/bin/lmce-game_maintenance ] && /usr/bin/lmce-game_maintenance
