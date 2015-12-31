#
# Regular cron jobs for the pluto-plutoutils package
#
0 4	* * *	root	[ -x /usr/bin/pluto-plutoutils_maintenance ] && /usr/bin/pluto-plutoutils_maintenance
