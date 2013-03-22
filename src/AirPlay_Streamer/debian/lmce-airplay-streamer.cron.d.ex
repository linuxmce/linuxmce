#
# Regular cron jobs for the lmce-airplay-streamer package
#
0 4	* * *	root	[ -x /usr/bin/lmce-airplay-streamer_maintenance ] && /usr/bin/lmce-airplay-streamer_maintenance
