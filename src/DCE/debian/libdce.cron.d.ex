#
# Regular cron jobs for the libdce package
#
0 4	* * *	root	[ -x /usr/bin/libdce_maintenance ] && /usr/bin/libdce_maintenance
