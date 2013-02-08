#
# Regular cron jobs for the lmce-mythtv-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-mythtv-plugin_maintenance ] && /usr/bin/lmce-mythtv-plugin_maintenance
