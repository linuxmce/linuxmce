#!/bin/bash
export LC_ALL="C"

. /usr/pluto/bin/Config_Ops.sh

UPDATE_CACHE=""
HOST_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
HOST_RELEASE=$(lsb_release -c -s)
HOST_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
DEB_CACHE="${HOST_DISTRO}-${HOST_RELEASE}-${HOST_ARCH}"

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

echo "Processing :: Updating all deb-cache locations"

mkdir -p /usr/pluto/deb-cache/${DEB_CACHE}

# update sources.list for any old/existing entries
if [[ ! $(grep ${DEB_CACHE} /etc/apt/sources.list) ]] ; then
	# fix sources.list, replace "/deb-cache " with "/deb-cache/${DEB_CACHE} "
	sed -i "s:/deb-cache :/deb-cache/${DEB_CACHE} :" /etc/apt/sources.list
fi

# clear the original packages lists to avoid problems.
rm -f /usr/pluto/deb-cache/Packages*
touch /usr/pluto/deb-cache/Packages
gzip -9c < /usr/pluto/deb-cache/Packages > /usr/pluto/deb-cache/Packages.gz

if [[ "$PK_Device" == "1" ]] || mountpoint -q /usr/pluto/deb-cache/ ; then
	# move files to new deb_cache
	if [[ -n "$(find /usr/pluto/deb-cache/ -maxdepth 1 -iname '*.deb')" ]]; then
		echo "Moving the old deb-cache to /usr/pluto/deb-cache/${DEB_CACHE}"
		find /usr/pluto/deb-cache/ -maxdepth 1 -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/${DEB_CACHE} \;
		UPDATE_CACHE="${DEB_CACHE}"
	fi
	if [[ -n "$(find /var/cache/apt/archives/ -maxdepth 1 -iname '*.deb')" ]]; then
		echo "Moving the apt cache to /usr/pluto/deb-cache/${DEB_CACHE}"
		find /var/cache/apt/archives/ -maxdepth 1 -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/${DEB_CACHE} \;
		UPDATE_CACHE="${DEB_CACHE}"
	fi
fi

# add any empty dirs in deb-cache so they get Packages files as well
for cache in $(find /usr/pluto/deb-cache/ -depth -type d -empty 2>/dev/null) ; do
	[[ -n "$UPDATE_CACHE" ]] && UPDATE_CACHE="${UPDATE_CACHE} $(basename ${cache})" || UPDATE_CACHE="$(basename ${cache})"
done

for Moon_RootLocation in $(find /usr/pluto/diskless/* -maxdepth 0 -type d 2>/dev/null); do
if [[ -n "$Moon_RootLocation" && -e "$Moon_RootLocation/etc/pluto.conf" ]]; then
	TARGET_DISTRO=$(LC_ALL="C" chroot ${Moon_RootLocation} lsb_release -i -s | tr '[:upper:]' '[:lower:]')
	TARGET_RELEASE=$(LC_ALL="C" chroot ${Moon_RootLocation} lsb_release -c -s)
	TARGET_ARCH=$(LC_ALL="C" chroot ${Moon_RootLocation} apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
	DEB_CACHE="${TARGET_DISTRO}-${TARGET_RELEASE}-${TARGET_ARCH}"

	mkdir -p ${Moon_RootLocation}/usr/pluto/deb-cache/${DEB_CACHE}

	if [[ -n "$(find ${Moon_RootLocation}/usr/pluto/deb-cache/ -maxdepth 1 -iname '*.deb')" ]]; then
		echo "Moving the MD's deb-cache to /usr/pluto/deb-cache/${DEB_CACHE}"
		find ${Moon_RootLocation}/usr/pluto/deb-cache/ -maxdepth 1 -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/${DEB_CACHE} \;
		if [[ "$UPDATE_CACHE" != "*${DEB_CACHE}*" ]]; then
			UPDATE_CACHE="$UPDATE_CACHE ${DEB_CACHE}"
		fi
	fi
	if [[ -n "$(find ${Moon_RootLocation}/var/cache/apt/archives/ -maxdepth 1 -iname '*.deb')" ]]; then
		echo "Moving this MDs apt cache to /usr/pluto/deb-cache/${DEB_CACHE}"
		find ${Moon_RootLocation}/var/cache/apt/archives/ -maxdepth 1 -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/${DEB_CACHE} \;
		if [[ "$UPDATE_CACHE" != "*${DEB_CACHE}*" ]]; then
			UPDATE_CACHE="$UPDATE_CACHE ${DEB_CACHE}"
		fi
	fi
	if [[ -n "$(find ${Moon_RootLocation}/usr/pluto/deb-cache/${DEB_CACHE} -maxdepth 1 -iname '*.deb')" ]]; then
		echo "Moving the MD's deb-cache to /usr/pluto/deb-cache/${DEB_CACHE}"
		find ${Moon_RootLocation}/usr/pluto/deb-cache/${DEB_CACHE} -maxdepth 1 -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/${DEB_CACHE} \;
		if [[ "$UPDATE_CACHE" != "*${DEB_CACHE}*" ]]; then
			UPDATE_CACHE="$UPDATE_CACHE ${DEB_CACHE}"
		fi
	fi

	# update sources.list for any old/existing entries
	if [[ ! $(grep ${DEB_CACHE} ${Moon_RootLocation}/etc/apt/sources.list) ]] ; then
		# fix sources.list, replace "/deb-cache " with "/deb-cache/${DEB_CACHE} "
		sed -i "s:/deb-cache :/deb-cache/${DEB_CACHE} :" ${Moon_RootLocation}/etc/apt/sources.list
	fi

	# clear the original packages lists to avoid problems.
	rm -f ${Moon_RootLocation}/usr/pluto/deb-cache/Packages*
	touch ${Moon_RootLocation}/usr/pluto/deb-cache/Packages
	gzip -9c < ${Moon_RootLocation}/usr/pluto/deb-cache/Packages > ${Moon_RootLocation}/usr/pluto/deb-cache/Packages.gz

	# create a blank packages list on the MD so apt-get update does not fail
	rm -f ${Moon_RootLocation}/usr/pluto/deb-cache/${DEB_CACHE}/Packages*
	touch ${Moon_RootLocation}/usr/pluto/deb-cache/${DEB_CACHE}/Packages
	gzip -9c < ${Moon_RootLocation}/usr/pluto/deb-cache/${DEB_CACHE}/Packages > ${Moon_RootLocation}/usr/pluto/deb-cache/${DEB_CACHE}/Packages.gz
fi
done

# update package files
for cache_name in "$UPDATE_CACHE"; do
if [[ -n "$cache_name" ]]; then
        echo "Updating packages list in /usr/pluto/deb-cache/$cache_name"
        update_debcache /usr/pluto/deb-cache/$cache_name || :
fi
done

echo "Processing :: deb-cache updates complete."
