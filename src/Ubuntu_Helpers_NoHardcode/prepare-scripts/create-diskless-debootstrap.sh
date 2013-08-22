#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh

set -e

echo ""
echo "********************************************************************************"
echo "*** Running: $0"
echo "********************************************************************************"

function do_debootstrap {
echo do debootstrap
	local build_name="$1"
	local temp_dir="$2"
	local repository="$3"

	qemu_arch=""
	case "$arch" in
		i386)
		qemu_arch="$arch"
		;;
		amd64)
		qemu_arch="x86_64"
		;;
		armel|armhf)
		qemu_arch="arm"
		;;
	esac

	[ `which "qemu-$qemu_arch-static"` ] && qemu_static_bin=$(which "qemu-$qemu_arch-static")
	debootstrap --arch "$arch" --foreign "$build_name" "$temp_dir" "$repository"
	mkdir -p "$temp_dir/usr/bin"
	[[ -f "$qemu_static_bin" ]] && cp "$qemu_static_bin" "$temp_dir/usr/bin"
	chroot "$temp_dir" /debootstrap/debootstrap --second-stage
}

function Create_Diskless_Debootstrap_Archive {
	# Create a temp dir to store a diskless debootstrap image into
	local temp_dir="$(mktemp -d)"

	# Run debootstrap in that dir
	do_debootstrap "${build_name}" "$temp_dir" "$repository"

	# Clean it's apt cache so we don't end up with it in the archive
	rm -f $temp_dir/var/cache/apt/archives/*.deb
	
	# Add some needed repos, if required
	if [[ ! -z "$additional_repos" ]]; then
		echo "$additional_repos"  >$temp_dir/etc/apt/sources.list
	fi
	
	echo "192.168.80.1 dcerouter" >>$temp_dir/etc/hosts

	# And make sure, the latest repos info is available
	chroot $temp_dir apt-get update

	if [ -e "$temp_dir/etc/default/resolveconf/resolv.conf.d/original" ]; then
		rm -f "$temp_dir/etc/default/resolveconf/resolv.conf.d/original"
	fi
	if [ -e "$temp_dir/etc/default/resolveconf/resolv.conf.d/tail" ]; then
		rm -f "$temp_dir/etc/default/resolveconf/resolv.conf.d/tail"
	fi

	# Pack the diskless image in the right dir
	mkdir -p "$diskless_dir"
	pushd "$temp_dir"
		tar -jcf "$diskless_dir/$diskless_image_name" *
	popd
	
	# Remove the temp dir
	rm -rf "$temp_dir"	
}

case "$flavor" in
	"ubuntu")
	repository="http://archive.ubuntu.com/ubuntu/"
	diskless_image_name="PlutoMD_Debootstraped.tar.bz2"
	additional_repos="deb $repository $build_name main restricted universe multiverse
deb http://security.ubuntu.com/ubuntu/ $build_name main restricted universe multiverse"
	;;
	"raspbian")
	repository="http://archive.raspbian.org/raspbian/"
	diskless_image_name="PlutoMD_Debootstraped-$flavor-$build_name-$arch.tar.bz2"
	additional_repos="deb $repository $build_name rpi"
	;;
esac

Create_Diskless_Debootstrap_Archive


echo "*** Done: $0"

