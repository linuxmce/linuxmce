#
# Regular cron jobs for the pluto-std-plugins package
#
0 4	* * *	root	[ -x /usr/bin/pluto-std-plugins_maintenance ] && /usr/bin/pluto-std-plugins_maintenance
