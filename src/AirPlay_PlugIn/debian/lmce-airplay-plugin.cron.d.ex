#
# Regular cron jobs for the lmce-airplay-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-airplay-plugin_maintenance ] && /usr/bin/lmce-airplay-plugin_maintenance
