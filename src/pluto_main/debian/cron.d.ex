#
# Regular cron jobs for the libpluto-main package
#
0 4	* * *	root	[ -x /usr/bin/libpluto-main_maintenance ] && /usr/bin/libpluto-main_maintenance
