#
# Regular cron jobs for the lmce-rpc-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-rpc-plugin_maintenance ] && /usr/bin/lmce-rpc-plugin_maintenance
