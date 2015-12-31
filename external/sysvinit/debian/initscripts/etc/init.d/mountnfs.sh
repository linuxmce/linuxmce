#
# mountnfs.sh	Now that TCP/IP is configured, mount the NFS file
#		systems in /etc/fstab if needed. If possible,
#		start the portmapper before mounting (this is needed for
#		Linux 2.1.x and up).
#
#		Also mounts SMB filesystems now, so the name of
#		this script is getting increasingly inaccurate.
#
# Version:	@(#)mountnfs.sh  2.86-5  10-Sep-2004  miquels@cistron.nl
#

VERBOSE=yes
TMPTIME=0
[ -f /etc/default/rcS ] && . /etc/default/rcS
. /etc/init.d/bootclean.sh

#
#	Run in a subshell because of I/O redirection.
#
test -f /etc/fstab && (

#
#	Read through fstab line by line. If it is NFS, set the flag
#	for mounting NFS file systems. If any NFS partition is found and it
#	not mounted with the nolock option, we start the portmapper.
#
portmap=no
while read device mountpt fstype options
do
	case "$device" in
		""|\#*)
			continue
			;;
	esac

	case "$options" in
		*noauto*)
			continue
			;;
	esac

	case "$fstype" in
		nfs|nfs4)
			case "$options" in
				*nolock*)
					;;
				*)
					portmap=yes
					;;
			esac
			;;
		smbfs|cifs|coda|ncp|ncpfs)
			;;
		*)
			fstype=
			;;
	esac
	if [ -n "$fstype" ]
	then
		case "$NETFS" in
			$fstype|*,$fstype|$fstype,*|*,$fstype,*)
				;;
			*)
				NETFS="$NETFS${NETFS:+,}$fstype"
				;;
		esac
	fi
done

exec 0>&1

if [ "$portmap" = yes ]
then
	if [ -x /sbin/portmap ] && [ -z "`pidof portmap`" ] 
	then
		echo -n "Starting portmapper..."
		if [ -x /etc/init.d/portmap ] 
		then
			/etc/init.d/portmap start
	        else
			start-stop-daemon --start --quiet --exec /sbin/portmap
			sleep 2
		fi
	fi
fi

if [ -n "$NETFS" ]
then
	echo "Mounting remote filesystems..."
	mount -a -t$NETFS
fi

) < /etc/fstab

#
#       Clean /tmp, /var/lock, /var/run
#
bootclean mountnfs

: exit 0

