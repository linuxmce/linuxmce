#
# Regular cron jobs for the updatemedia package
#
0 4	* * *	root	[ -x /usr/bin/updatemedia_maintenance ] && /usr/bin/updatemedia_maintenance
