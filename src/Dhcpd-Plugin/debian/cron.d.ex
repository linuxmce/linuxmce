#
# Regular cron jobs for the lmce-dhcpd-plugin package
#
0 4	* * *	root	[ -x /usr/bin/lmce-dhcpd-plugin_maintenance ] && /usr/bin/lmce-dhcpd-plugin_maintenance
