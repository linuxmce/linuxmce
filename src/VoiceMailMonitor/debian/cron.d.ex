#
# Regular cron jobs for the pluto-voicemail-monitor package
#
0 4	* * *	root	[ -x /usr/bin/pluto-voicemail-monitor_maintenance ] && /usr/bin/pluto-voicemail-monitor_maintenance
