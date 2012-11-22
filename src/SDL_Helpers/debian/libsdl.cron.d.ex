#
# Regular cron jobs for the libsdl package
#
0 4	* * *	root	[ -x /usr/bin/libsdl_maintenance ] && /usr/bin/libsdl_maintenance
