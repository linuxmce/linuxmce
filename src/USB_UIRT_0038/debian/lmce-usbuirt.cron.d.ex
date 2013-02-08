#
# Regular cron jobs for the lmce-usbuirt package
#
0 4	* * *	root	[ -x /usr/bin/lmce-usbuirt_maintenance ] && /usr/bin/lmce-usbuirt_maintenance
