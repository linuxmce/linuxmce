#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh

set -e

DEVICEDATA_DisklessImages=258
DEVICEDATA_Architecture=112

if [[ "$#" -ne 1 ]]; then
	echo "Syntax: $0 <Device>"
	exit 1
fi

Moon_DeviceID="$1"
if [[ "$Moon_DeviceID" == "" ]] ;then
	echo "ERROR: Script $0 called with missing parameter"
	exit 1
fi

Moon_RootLocation="/usr/pluto/diskless/${Moon_DeviceID}"
Moon_Architecture=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_Architecture'")

if [[ -z "$Moon_Architecture" ]]; then
	Moon_Architecture=i386
fi

Moon_RootArchive="/usr/pluto/install/PlutoMD-$Moon_Architecture.tar.bz2"
DisklessImages=$(GetDeviceData "$Moon_DeviceID" "$DEVICEDATA_DisklessImages")

echo "INFO ::::: root:archive = $Moon_RootArchive"
echo "INFO ::::: disklessimage = $DisklessImages"

function unpack_filesystem {
	echo "INFO : Unpacking filesystem"
#	local DisklessImages=$(GetDeviceData "$Moon_DeviceID" "$DEVICEDATA_DisklessImages")
	local NameKernel Kernel Name Value
	if [[ -n "$DisklessImages" ]]; then
		if [[ -d "$Moon_RootLocation" ]]; then
			return 0
		fi
		for line in $DisklessImages; do
			NameKernel="${line%%=*}"
			if [[ "$NameKernel" == *-* ]]; then
				Name="${NameKernel%%-*}"
				Kernel="${NameKernel#*-}"
			else
				Name="$NameKernel"
				Kernel=
			fi
			Value="${line#*=}"
			mkdir -p "$Moon_RootLocation/$Name"
			pushd "$Moon_RootLocation/$Name"
				if [[ ! -f "/usr/pluto/install/$Value" ]]; then
					echo "INFO : Diskless archive not found"
					exit 1
				else
					tar -xf "/usr/pluto/install/$Value"
				fi
			popd
		done
	elif [[ ! -e ${Moon_RootLocation}/var/lib/dpkg/status ]] ;then
		mkdir -p "${Moon_RootLocation}"	
		pushd $Moon_RootLocation
			tar -xf $Moon_RootArchive
		popd
	fi
	# Copy core's apt.conf.d/02proxy to generated MD
	if [[ -e /etc/apt/apt.conf.d/02proxy ]] ;then
	  cp /etc/apt/apt.conf.d/02proxy ${Moon_RootLocation}/etc/apt/apt.conf.d/02proxy
	fi
}

function is_image_upgradable {
	echo "INFO : Testing if diskimage is upgradeable"
	## Test to see if the image is upgradable
	cp /etc/resolv.conf ${Moon_RootLocation}/etc/resolv.conf

#	[[ -f /etc/apt/apt.conf ]] &&  cp /etc/apt/apt.conf ${Moon_RootLocation}/etc/apt/apt.conf
#	cp /etc/apt/sources.list ${Moon_RootLocation}/etc/apt/sources.list

	chroot $Moon_RootLocation apt-get update 2>/dev/null 1>/dev/null
	UpgradablePkgs=$(chroot $Moon_RootLocation apt-get -s dist-upgrade 2>/dev/null | grep "^Conf\|^Inst" | wc -l)
	
	if [[ "$UpgradablePkgs" != "0" ]] ;then
		return 0
	else
		return 1
	fi
}

function create_diskless_conf {
	echo "INFO : Creating ${Moon_RootLocation}/etc/diskless.conf"
	## Create /etc/diskless.conf
	local R=$(RunSQL "SELECT IPaddress,MACaddress FROM Device WHERE PK_Device='$Moon_DeviceID'")
	local IP=$(Field 1 "$R")
	local MAC=$(Field 2 "$R")
	conf="IP=$IP
MAC=$MAC
Device=$Moon_DeviceID
Code="
	echo "$conf" >${Moon_RootLocation}/etc/diskless.conf

}

if [[ ! -f $Moon_RootArchive && -z "$DisklessImages" ]]; then
	echo "INFO : Diskless archive not found or not specified, exiting"
	exit 1
#	/usr/pluto/bin/Diskless_CreateTBZ.sh
fi

echo "INFO : Unpacking diskless archive"
unpack_filesystem
create_diskless_conf	
