#
# Regular cron jobs for the lmce-videolan-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-videolan-plugin_maintenance ] && /usr/bin/lmce-videolan-plugin_maintenance
