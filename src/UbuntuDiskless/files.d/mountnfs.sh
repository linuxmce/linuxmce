#!/bin/bash
set -e 

. ./filesd-parms.sh $@

File="/etc/init.d/mountnfs.sh"

INSSERV="/sbin/insserv"
if ! [[ -x "${Parm_RootLocation}$INSSERV" ]]; then
        INSSERV="/usr/lib/insserv/insserv"
fi
if ! [[ -x "${Parm_RootLocation}$INSSERV" ]]; then
        echo "No insserv found.  Exiting."
        exit 1
fi

mkdir -p "${Parm_RootLocation}/$(dirname $File)"
cat >"${Parm_RootLocation}/$File" <<"EOF"
#!/bin/sh
### BEGIN INIT INFO
# Provides:          mountnfs.sh
# Required-Start:    $remote_fs
# Required-Stop:     $remote_fs
# Should-Start:
# Default-Start:     S 2
# Default-Stop:
# Short-Description: mountnfs.sh
# Description:       This script is run on MDs to ensure all remote mounts are in place
### END INIT INFO #

do_start () {
	mount -a -t nfs,nfs4,smbfs,cifs,ncp,ncpfs,coda,ocfs2,gfs
}

do_status () {
	return 0
}

case "$1" in
	start|"")
		do_start
		;;
	restart|reload|force-reload)
		echo "Error: argument '$1' not supported" >&2
		exit 3
                ;;
	stop)
		# No-op
		;;
	status)
		do_status
		exit $?
		;;
	*)
		echo "Usage: $0 [start|stop|status]" >&2
		exit 3
		;;
esac
:
EOF

chmod +x "${Parm_RootLocation}/${File}"
#LC_ALL=C chroot "${Parm_RootLocation}" insserv -fv mountnfs.sh
LC_ALL=C chroot "${Parm_RootLocation}" "${INSSERV}" -fv mountnfs.sh

## This script is broken so we better remove it
rm -f "${Parm_RootLocation}"/etc/init.d/waitnfs.sh

