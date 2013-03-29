#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh

set -e

DEVICEDATA_DisklessImages=258

if [[ "$#" -ne 1 ]]; then
	echo "Syntax: $0 <Device>"
	exit 1
fi
Moon_DeviceID="$1"
Moon_RootLocation="/usr/pluto/diskless/${Moon_DeviceID}"

DisklessImages=$(GetDeviceData "$Moon_DeviceID" "$DEVICEDATA_DisklessImages")
if [[ -n "$DisklessImages" ]]; then
	exit 0
fi

## Determine what kernel are we going to use
# use the installed kernel on the MD, not the core's running kernel
#Moon_KernelVersion=$(uname -r)
Moon_KernelVersion=$(basename $(ls /boot/vmlinuz-* | head -1) | cut -d"-" -f2-99)

## Create symlinks to existing kernels
rm -f ${Moon_RootLocation}/boot/initrd.img
ln -s ${Moon_RootLocation}/boot/initrd.img-${Moon_KernelVersion} ${Moon_RootLocation}/boot/initrd.img
rm -f ${Moon_RootLocation}/boot/vmlinuz
ln -s ${Moon_RootLocation}/boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/boot/vmlinuz
