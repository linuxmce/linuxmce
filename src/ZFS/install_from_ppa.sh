#!/bin/bash

#
# Configuration script for : Adding ZFS to a system
#

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh

DD_DISTRO_Precise=20
DD_DISTRO_Trusty=21
DD_DISTRO_Jessie=22
DD_DISTRO_Xenial=23

#
# Install functions
#

install_from_stable_ppa()
{
    if ! PackageIsInstalled "ubuntu-zfs"; then
	echo "y" | add-apt-repository ppa:zfs-native/stable
	apt-get update
	apt-get -y install ubuntu-zfs
	apt-get -y install zfs-zed
    fi

    modprobe zfs
}

install_xenial()
{
    if ! PackageIsInstalled "zfs"; then
	apt-get -y install zfs
	apt-get -y install zfs-zed
    fi

    modprobe zfs

}


#
# main()
#

#
# Install the ZFS On Linux bits and enable them, set a lock
# for the web admin to wait for this to finish.
#

Lock "ZFS" "ZFS"

case $PK_Distro in
    $DD_DISTRO_Precise) install_from_stable_ppa ;;
    $DD_DISTRO_Trusty) install_from_stable_ppa ;;
    $DD_DISTRO_Jessie) exit 0 ;;
    $DD_DISTRO_Xenial) install_xenial ;;
esac

Unlock "ZFS" "ZFS"

