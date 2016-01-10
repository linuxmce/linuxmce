#
# Regular cron jobs for the libbdcommon package
#
0 4	* * *	root	[ -x /usr/bin/libbdcommon_maintenance ] && /usr/bin/libbdcommon_maintenance
