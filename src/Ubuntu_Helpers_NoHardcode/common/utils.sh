#!/bin/bash

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
				if dpkg --compare-versions "$dup_ver" lt "$ver" ;then
					echo "Remove duplicate package $pkg $dup_ver $dup_arch"
					rm -f "${pkg}_${dup_ver}_"*.deb
					rm -f "${pkg}_${dup_ver}_"*.changes || :
					rm -f "${pkg}_${dup_ver}_"*.upload || :
				fi

				if dpkg --compare-versions "$dup_ver" gt "$ver" ;then
					echo "Remove duplicate package $pkg $ver $arch"
					rm -f "${pkg}_${ver}_"*.deb
					rm -f "${pkg}_${dup_ver}_"*.changes || :
					rm -f "${pkg}_${dup_ver}_"*.upload || :
					ver=$dup_ver
				fi
			fi
		done
	done
	popd
}
