#
# Regular cron jobs for the lmce-infrared-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-infrared-plugin_maintenance ] && /usr/bin/lmce-infrared-plugin_maintenance
