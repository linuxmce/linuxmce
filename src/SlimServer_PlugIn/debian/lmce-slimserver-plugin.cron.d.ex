#
# Regular cron jobs for the lmce-slimserver-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-slimserver-plugin_maintenance ] && /usr/bin/lmce-slimserver-plugin_maintenance
