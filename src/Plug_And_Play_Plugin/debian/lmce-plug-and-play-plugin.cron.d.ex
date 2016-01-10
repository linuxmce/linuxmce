#
# Regular cron jobs for the lmce-plug-and-play-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-plug-and-play-plugin_maintenance ] && /usr/bin/lmce-plug-and-play-plugin_maintenance
