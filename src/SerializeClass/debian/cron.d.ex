#
# Regular cron jobs for the libserialize package
#
0 4	* * *	root	[ -x /usr/bin/libserialize_maintenance ] && /usr/bin/libserialize_maintenance
