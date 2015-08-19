#!/bin/bash
export LC_ALL="C"

HOST_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
HOST_RELEASE=$(lsb_release -c -s)
HOST_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)

remove_duplicate_debs() {
        local Dir="$1"
        local pkg ver arch dup_ver dup_arch

        pushd "$Dir" >/dev/null
        for deb in *.deb ;do
                pkg=$(echo $deb | cut -d'_' -f1)
                ver=$(echo $deb | cut -d'_' -f2)
                arch=$(echo $deb | cut -d '_' -f3 | cut -d'.' -f1)

                for dup_deb in ${pkg}_*.deb ;do
                        dup_ver=$(echo $dup_deb | cut -d'_' -f2)
                        dup_arch=$(echo $dup_deb | cut -d '_' -f3 | cut -d'.' -f1)

                        if [[ "$dup_arch" == "$arch" ]] ;then
                                if dpkg --compare-versions "$dup_ver" lt "$ver" 2>/dev/null ;then
                                        echo "Remove duplicate package $pkg $dup_ver $dup_arch"
                                        rm -f "${pkg}_${dup_ver}_"*.deb
                                fi

                                if dpkg --compare-versions "$dup_ver" gt "$ver" 2>/dev/null ;then
                                        echo "Remove duplicate package $pkg $ver $arch"
                                        rm -f "${pkg}_${ver}_"*.deb
                                        ver=$dup_ver
                                fi
                        fi
                done
        done
        popd >/dev/null
}

update_debcache() {
	local Dir="$1"

	mkdir -p "$Dir"
	pushd "$Dir" >/dev/null
	remove_duplicate_debs "./"
	dpkg-scanpackages ./ /dev/null 2>/dev/null | tee Packages | gzip -9c > Packages.gz
	popd >/dev/null
}

if [[ -n "$1" ]]; then
	[[ -d "$1" ]] && update_debcache "$1"
	exit 0
fi

echo "Processing :: Updating deb-cache locations"

rm -f /tmp/target_updates
touch /tmp/target_updates

for disklessMD in /usr/pluto/diskless/* ; do
	if [[ -d "$disklessMD" ]] ; then
		#echo Processing :: $disklessMD

		chroot "$disklessMD" lsb_release -is > /tmp/target_distro
		TARGET_DISTRO=$(cat /tmp/target_distro)
		#echo $TARGET_DISTRO

		chroot "$disklessMD" lsb_release -cs > /tmp/target_release
		TARGET_RELEASE=$(cat /tmp/target_release)
		#echo $TARGET_RELEASE

		#TARGET_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
		chroot "$disklessMD" apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1 > /tmp/target_arch
		TARGET_ARCH=$(cat /tmp/target_arch)
		#echo $TARGET_ARCH

		DEB_CACHE=${TARGET_DISTRO,,}-${TARGET_RELEASE}-${TARGET_ARCH}

		if [[ -n "$(find $disklessMD/var/cache/apt/archives/ -maxdepth 1 -iname '*.deb')" ]]; then
			echo "Processing :: Moving $disklessMD's apt cache to /usr/pluto/deb-cache/$DEB_CACHE"
			find "$disklessMD"/var/cache/apt/archives/ -maxdepth 1 -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/$DEB_CACHE \;
			echo "$DEB_CACHE" >> /tmp/target_updates
		fi
	fi
done
for debcache in $( cat /tmp/target_updates | sort -u ) ; do
	echo "Processing :: Building Packages files in /usr/pluto/deb-cache/$debcache"
	update_debcache /usr/pluto/deb-cache/$debcache
done

echo "Processing :: deb-cache updates complete."
