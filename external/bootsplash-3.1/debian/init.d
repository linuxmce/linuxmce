#!/bin/sh
# based on initscript from gentoo linux
# made some changes to fit normal sysvinit like used by debian
# everything in here is GPL ;)
# tcs@double-action.org

# source our config
source /etc/default/bootsplash 2>/dev/null

# default settings
test -z "${THEME}" && THEME="default"
test -z "${BOOTSPLASH_TTYS}"  && BOOTSPLASH_TTYS=`seq 0 5`

get_bootsplash_theme () {
	# try to get bootsplash theme from kernel command line
	for param in `grep "theme=" /proc/cmdline`; do
		if [ "${param%=*}" == "theme" ]; then
			THEME="${param#*=}"
		fi
	done
}

SPLASH=$(which splash)

case "$1" in
start)
	# Only do this if the kernel has support
	if [ -f /proc/splash ]
	then
		echo "Setting Console frame buffer images"

		get_bootsplash_theme
		echo "Using theme $THEME"
		# get console resolution
		RESOLUTION=$(/sbin/fbresolution)

		# switch to a usable image on all consoles
		for TTY in `echo "${BOOTSPLASH_TTYS}" | sed -e 's# #\n#g'`
		do
			# support for a different config per virtual terminal
			if [ -e /etc/bootsplash/themes/${THEME}/config/vtsplash-${TTY}-${RESOLUTION}.cfg ]
			then
				$SPLASH -s -u $TTY -n /etc/bootsplash/themes/${THEME}/config/vtsplash-${TTY}-${RESOLUTION}.cfg
			else
				$SPLASH -s -u $TTY -n /etc/bootsplash/themes/${THEME}/config/bootsplash-${RESOLUTION}.cfg
			fi
		done
	fi
;;
stop)
	CHVT=$(which chvt)
	# Stop doesn't really stop, it actually changes the image
	# on vt1 back to the bootsplash image.
	# Only do this if the kernel has support
	if [ -f /proc/splash ]
	then
		echo "Setting Console frame buffer images"
		get_bootsplash_theme
		# source bootsplash config
		source /etc/bootsplash/themes/${THEME}/config/bootsplash-$(/sbin/fbresolution).cfg 2>/dev/null
		# Put bootsplash image up
		$SPLASH -s -u 0 /etc/bootsplash/themes/${THEME}/config/bootsplash-$(/sbin/fbresolution).cfg
		# ensure "silent" image is displayed
		echo "silent" > /proc/splash
		# switch to vt1 (must be &'d otherwise script hangs)
		${CHVT} 1 &
		# print a nice message
		# /sbin/fbtruetype ........
	fi
;;
esac

exit 0
