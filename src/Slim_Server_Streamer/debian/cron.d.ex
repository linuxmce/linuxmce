#
# Regular cron jobs for the pluto-slim-server-streamer package
#
0 4	* * *	root	[ -x /usr/bin/pluto-slim-server-streamer_maintenance ] && /usr/bin/pluto-slim-server-streamer_maintenance
