#
# Regular cron jobs for the lmce-shoutcast-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-shoutcast-plugin_maintenance ] && /usr/bin/lmce-shoutcast-plugin_maintenance
