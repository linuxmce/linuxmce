#
# Regular cron jobs for the lmce-telecom-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-telecom-plugin_maintenance ] && /usr/bin/lmce-telecom-plugin_maintenance
