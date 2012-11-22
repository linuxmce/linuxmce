#
# Regular cron jobs for the lmce-datalogger-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-datalogger-plugin_maintenance ] && /usr/bin/lmce-datalogger-plugin_maintenance
