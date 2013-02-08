#
# Regular cron jobs for the lmce-orbiter-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-orbiter-plugin_maintenance ] && /usr/bin/lmce-orbiter-plugin_maintenance
