#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh

set -e

echo ""
echo "********************************************************************************"
echo "*** Running: $0"
echo "********************************************************************************"

function Create_Diskless_Debootstrap_Archive {
	# Create a temp dir to store a diskless debootstrap image into
	local temp_dir="$(mktemp -d)"

	# Run debootstrap in that dir
	debootstrap "${build_name}" "$temp_dir" http://archive.ubuntu.com/ubuntu/

	# Clean it's apt cache so we don't end up with it in the archive
	rm -f $temp_dir/var/cache/apt/archives/*.deb
	
	# Add some needed repos
	echo deb http://archive.ubuntu.com/ubuntu/ $build_name main restricted universe multiverse >$temp_dir/etc/apt/sources.list
	echo deb http://security.ubuntu.com/ubuntu/ $build_name main restricted universe multiverse >>$temp_dir/etc/apt/sources.list
#	echo deb http://www.avenard.org/files/ubuntu-repos $build_name release >>$temp_dir/etc/apt/sources.list
#	echo deb http://deb.linuxmce.org/ubuntu/  ${DISTRO}  ${COMPOS} >$temp_dir/etc/apt/sources.list
	
	echo "192.168.80.1 dcerouter" >>$temp_dir/etc/hosts

	# And make sure, the latest repos info is available
	chroot $temp_dir apt-get update

	# Pack the diskless image in the right dir
	mkdir -p "$diskless_dir"
	pushd "$temp_dir"
		tar -jcf "$diskless_dir/PlutoMD_Debootstraped.tar.bz2" *
	popd
	
	# Remove the temp dir
	rm -rf "$temp_dir"	
}


Create_Diskless_Debootstrap_Archive


echo "*** Done: $0"

