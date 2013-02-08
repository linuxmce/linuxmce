#
# Regular cron jobs for the lmce-pandora-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-pandora-plugin_maintenance ] && /usr/bin/lmce-pandora-plugin_maintenance
