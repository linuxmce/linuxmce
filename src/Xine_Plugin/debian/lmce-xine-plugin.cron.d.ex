#
# Regular cron jobs for the lmce-xine-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-xine-plugin_maintenance ] && /usr/bin/lmce-xine-plugin_maintenance
