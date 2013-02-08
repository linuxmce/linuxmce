#
# Regular cron jobs for the lmce-windowsutils package
#
0 4	* * *	root	[ -x /usr/bin/lmce-windowsutils_maintenance ] && /usr/bin/lmce-windowsutils_maintenance
