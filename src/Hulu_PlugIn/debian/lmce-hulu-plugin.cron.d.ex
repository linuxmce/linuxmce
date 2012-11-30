#
# Regular cron jobs for the lmce-hulu-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-hulu-plugin_maintenance ] && /usr/bin/lmce-hulu-plugin_maintenance
