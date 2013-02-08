#
# Regular cron jobs for the libhupnp-av package
#
0 4	* * *	root	[ -x /usr/bin/libhupnp-av_maintenance ] && /usr/bin/libhupnp-av_maintenance
