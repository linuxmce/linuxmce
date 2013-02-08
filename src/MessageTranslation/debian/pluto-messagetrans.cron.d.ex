#
# Regular cron jobs for the pluto-messagetrans package
#
0 4	* * *	root	[ -x /usr/bin/pluto-messagetrans_maintenance ] && /usr/bin/pluto-messagetrans_maintenance
