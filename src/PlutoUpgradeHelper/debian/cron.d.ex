#
# Regular cron jobs for the pluto-upgrade-helper package
#
0 4	* * *	root	[ -x /usr/bin/pluto-upgrade-helper_maintenance ] && /usr/bin/pluto-upgrade-helper_maintenance
