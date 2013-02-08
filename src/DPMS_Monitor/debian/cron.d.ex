#
# Regular cron jobs for the lmce-dpms-monitor package
#
0 4	* * *	root	[ -x /usr/bin/lmce-dpms-monitor_maintenance ] && /usr/bin/lmce-dpms-monitor_maintenance
