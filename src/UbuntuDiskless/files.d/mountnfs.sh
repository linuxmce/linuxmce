#!/bin/bash
set -e 

. ./filesd-parms.sh $@

File="/etc/init.d/mountnfs.sh"

mkdir -p "${Parm_RootLocation}/$(dirname $File)"
cat <<-'EOF' >"${Parm_RootLocation}/$File"
	#!/bin/sh
	### BEGIN INIT INFO
	# Provides:          mountnfs.sh
	# Required-Start:    $remote_fs
	# Required-Stop:     $remote_fs
	# Should-Start:
	# Default-Start:     2 3 4 5
	# Default-Stop:
	# Short-Description: mountnfs.sh
	# Description:       This script is run on MDs to ensure all remote mounts are in place
	### END INIT INFO #

	# Source fuction library
	. /lib/lsb/init-functions

	start () {
	        log_daemon_msg "Mounting remote filesystems" "mountnfs.sh"
	        mount -a -t nfs,nfs4,smbfs,cifs,ncp,ncpfs,coda,ocfs2,gfs
                err=$?
	        log_end_msg $err
	        return $err
	}

	status () {
	        status_of_proc mountnfs.sh mountnfs.sh
	        return $?
	}

	case "$1" in
	        start|"")
	                $1
	                exit $?
	                ;;
	        restart|reload|force-reload)
	                echo "Error: argument '$1' not supported" >&2
	                exit 3
	                ;;
	        stop)
	                : # No-op
	                ;;
	        status)
	                $1
	                exit $?
	                ;;
	        *)
	                echo "Usage: $0 [start|stop|status]" >&2
	                exit 3
	esac
	:
	EOF
chmod +x "${Parm_RootLocation}/${File}"

LC_ALL=C chroot "${Parm_RootLocation}" update-rc.d mountnfs.sh defaults >/dev/null

