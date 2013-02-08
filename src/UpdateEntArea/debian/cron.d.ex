#
# Regular cron jobs for the pluto-updateentarea package
#
0 4	* * *	root	[ -x /usr/bin/pluto-updateentarea_maintenance ] && /usr/bin/pluto-updateentarea_maintenance
