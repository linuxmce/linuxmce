#
# Regular cron jobs for the lmce-xml-data-handler-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-xml-data-handler-plugin_maintenance ] && /usr/bin/lmce-xml-data-handler-plugin_maintenance
