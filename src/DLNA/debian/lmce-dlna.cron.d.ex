#
# Regular cron jobs for the lmce-dlna package
#
0 4	* * *	root	[ -x /usr/bin/lmce-dlna_maintenance ] && /usr/bin/lmce-dlna_maintenance
