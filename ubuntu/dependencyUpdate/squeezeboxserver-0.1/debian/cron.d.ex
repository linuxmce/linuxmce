#
# Regular cron jobs for the squeezeboxserver package
#
0 4	* * *	root	[ -x /usr/bin/squeezeboxserver_maintenance ] && /usr/bin/squeezeboxserver_maintenance
