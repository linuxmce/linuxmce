#
# Regular cron jobs for the lmce-lighting-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-lighting-plugin_maintenance ] && /usr/bin/lmce-lighting-plugin_maintenance
