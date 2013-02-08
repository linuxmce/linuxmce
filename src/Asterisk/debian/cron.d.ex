#
# Regular cron jobs for the pluto-asterisk package
#
0 4	* * *	root	[ -x /usr/bin/pluto-asterisk_maintenance ] && /usr/bin/pluto-asterisk_maintenance
