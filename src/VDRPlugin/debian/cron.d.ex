#
# Regular cron jobs for the pluto-vdr-plugin package
#
0 4	* * *	root	[ -x /usr/bin/pluto-vdr-plugin_maintenance ] && /usr/bin/pluto-vdr-plugin_maintenance
