#
# Regular cron jobs for the lmce-climate-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-climate-plugin_maintenance ] && /usr/bin/lmce-climate-plugin_maintenance
