#! /bin/sh
#
# umountnfs	Unmount all network filesystems except the root file system.
#
#		Also unmounts all virtual filesystems (proc, devfs,
#		devpts, usbfs, sysfs) that are not mounted at the
#		top-level.
#
# Version:      @(#)umountnfs  2.85-23  29-Jul-2004  miquels@cistron.nl
#

PATH=/sbin:/bin:/usr/sbin:/usr/bin

KERNEL=`uname -s`
RELEASE=`uname -r`

case "${KERNEL}:${RELEASE}" in
	Linux:[01].*|Linux:2.[01].*)
		FLAGS=""
		;;
	Linux:2.[23].*|Linux:2.4.?|Linux:2.4.?-*|Linux:2.4.10|Linux:2.4.10-*)
		FLAGS="-f"
		;;
	*)
		FLAGS="-f -l"
		;;
esac

# Write a reboot record to /var/log/wtmp before unmounting
halt -w

# Remove any .clean files in auto-clean dirs.
rm -f /tmp/.clean /var/lock/.clean /var/run/.clean

echo -n "Unmounting remote and non-toplevel virtual filesystems..."

#
#	Read mtab file and add the directories we want to
#	unmount in reverse to a list.
#
(
DIRS=""
	while read DEV DIR TYPE REST
	do
		case "$DIR" in
			/|/proc|/dev|/dev/pts|/proc/*|/sys)
				continue
				;;
		esac
		case "$TYPE" in
			nfs|nfs4|smbfs|ncp|ncpfs|cifs|coda)
				DIRS="$DIR $DIRS"
				;;
			proc|procfs|devfs|devpts|usbfs|usbdevfs|sysfs)
				DIRS="$DIR $DIRS"
				;;
		esac
	done
	exec </dev/null
	if [ -n "$DIRS" ]
	then
		umount $FLAGS $DIRS
	fi
) </etc/mtab

echo "done."

: exit 0
