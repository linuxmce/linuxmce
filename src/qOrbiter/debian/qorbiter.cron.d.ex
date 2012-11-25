#
# Regular cron jobs for the qorbiter package
#
0 4	* * *	root	[ -x /usr/bin/qorbiter_maintenance ] && /usr/bin/qorbiter_maintenance
