#!/bin/bash

. /tmp/mce_wizard_data.sh

FROM_CD=1
FROM_ISO=2


if [[ -z $1 ]] && [[ "$c_ubuntuLiveCdFrom" == "$FROM_ISO" ]] ;then
        [ -e "$c_ubuntuLiveCdPath" ] || exit 5
else

	CD_Dir=$(mktemp -d)
	Squash_Dir=$(mktemp -d)

	dpkg -i ./patch_*.deb
	dpkg -i ./dpkg-dev_*.deb
	dpkg -i ./dpkg-repack_*.deb

	#pt-get -f -y install dpkg-repack

	if [[ "$c_ubuntuLiveCdFrom" == "$FROM_ISO" ]] ;then
		lp=$(losetup -f)
	        losetup -d "$lp" || :
		mount -o loop "$c_ubuntuLiveCdPath" "$CD_Dir" || exit 1
	else
		mount /dev/cdrom "$CD_Dir" || exit 1
	fi

	if ! mount -o loop -t squashfs "$CD_Dir"/casper/filesystem.squashfs "$Squash_Dir"; then
		umount -f "$CD_Dir" || :
		exit 2
	fi

	mkdir -p /usr/pluto/deb-cache || exit 3
	pushd /usr/pluto/deb-cache >/dev/null
	COLUMNS=1024 chroot "$Squash_Dir" dpkg -l | awk 'NR>5 {print $2}' | xargs dpkg-repack --root="$Squash_Dir" || exit 4
	popd >/dev/null

	cp ./dpkg-scanpackages /usr/pluto/deb-cache || :
	cp ./controllib.pl /usr/pluto/deb-cache || :
	pushd /usr/pluto/deb-cache
		./dpkg-scanpackages ./ /dev/null > Packages
		gzip -c Packages > Packages.gz
	popd

	umount -f "$Squash_Dir" || :
	umount -f "$CD_Dir" || :
	lp=$(losetup -f)
	losetup -d "$lp" || :
	rm -rf "$Squash_Dir" "$CD_Dir"

	trap "" EXIT

	exit 0
fi
