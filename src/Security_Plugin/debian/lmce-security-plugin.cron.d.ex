#
# Regular cron jobs for the lmce-security-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-security-plugin_maintenance ] && /usr/bin/lmce-security-plugin_maintenance
