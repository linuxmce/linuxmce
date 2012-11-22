#
# Regular cron jobs for the lmce-datalog package
#
0 4	* * *	root	[ -x /usr/bin/lmce-datalog_maintenance ] && /usr/bin/lmce-datalog_maintenance
