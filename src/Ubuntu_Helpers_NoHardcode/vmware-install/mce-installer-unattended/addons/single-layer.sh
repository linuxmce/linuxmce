#!/bin/bash

function Determine_PkgNonGrata()
{
	## Description:
	# This function determines the intersection of two or more string lists

	if (("$#" < 2)); then
		return
	fi
	
	local i
	local LeftFile="$1" RightFile ResultFile
	sort -u "$LeftFile" >"$LeftFile.sort"
	LeftFile="$LeftFile.sort"

	for ((i = 2; i <= "$#"; i++)); do
		RightFile="${!i}"
		sort -u "$RightFile" >"$RightFile.sort"
		RightFile="$RightFile.sort"

		ResultFile="/tmp/result-$i"
		diff -U -1 "$LeftFile" "$RightFile" | tail -n +4 | grep -v '^[+-]' >"$ResultFile"

		rm -f "$LeftFile" "$RightFile"
		LeftFile="$ResultFile"
	done
	cat "$ResultFile"
}

# A list of packages that we never want to have in deb-cache
PkgNonGrata_Fixed="
	asterisk-pluto mce-diskless-tools pluto-asterisk pluto-avwizard-sounds pluto-dhcpd-plugin 
	pluto-orbiterinstaller pluto-skins-basic pluto-std-plugins pluto-xine-plugin
	pluto-xml-data-plugin video-wizard-videos pluto-avwizard pluto-dcerouter pluto-sample-media mysql-server-5.0
	openoffice.org-core wine pluto-orbiter python-kde3 pluto-default-tftpboot
"

# A list of pacakges that are installed on both the hybrid and MD
COLUMNS=1024 dpkg -l | awk '/^ii/ {print $2}' > /tmp/pkglist-hybrid.txt
PkgNonGrata_Determined=$(Determine_PkgNonGrata /tmp/pkglist-{diskless,hybrid}.txt)
rm -f /tmp/pkglist-{diskless,hybrid}.txt

# Remove the unneeded packages from deb-cache
for Pkg in $PkgNonGrata_Fixed $PkgNonGrata_Determined; do
	rm -f /usr/pluto/deb-cache/"$Pkg"_*.deb
done

# Rebuild the Package and Packages.gz files from deb-cache
(cd /usr/pluto/deb-cache && dpkg-scanpackages -m . /dev/null | tee Packages | gzip -c > Packages.gz)
