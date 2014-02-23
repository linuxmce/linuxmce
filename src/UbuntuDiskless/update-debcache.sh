#!/bin/bash

HOST_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
HOST_RELEASE=$(lsb_release -c -s)
HOST_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)


remove_duplicate_debs() {
        local Dir="$1"
        local pkg ver arch dup_ver dup_arch

        pushd "$Dir"
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
        popd
}

update_debcache() {
	local Dir="$1"

	mkdir -p "$Dir"
	pushd "$Dir"
	remove_duplicate_debs "./"
	dpkg-scanpackages ./ /dev/null 2>/dev/null | tee Packages | gzip -9c > Packages.gz
	popd
}

if [[ -n "$1" ]]; then
	update_debcache "$1"
	exit 0
fi

update_debcache /usr/pluto/deb-cache/$HOST_DISTRO-$HOST_RELEASE-$HOST_ARCH
