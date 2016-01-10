#
# Regular cron jobs for the pluto-hal-device-finder package
#
0 4	* * *	root	[ -x /usr/bin/pluto-hal-device-finder_maintenance ] && /usr/bin/pluto-hal-device-finder_maintenance
