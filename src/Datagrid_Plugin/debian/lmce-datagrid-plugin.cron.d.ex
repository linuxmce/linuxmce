#
# Regular cron jobs for the lmce-datagrid-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-datagrid-plugin_maintenance ] && /usr/bin/lmce-datagrid-plugin_maintenance
