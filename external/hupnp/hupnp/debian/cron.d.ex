#
# Regular cron jobs for the libhupnp-core package
#
0 4	* * *	root	[ -x /usr/bin/libhupnp-core_maintenance ] && /usr/bin/libhupnp-core_maintenance
