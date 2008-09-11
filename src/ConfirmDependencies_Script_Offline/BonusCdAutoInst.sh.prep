#!/bin/bash

BonusWorkDir="/usr/pluto/deb-cache/dists/sarge/main/binary-i386"

if [[ "$BonusCD" != N && "$BonusCD" != n ]]; then
	echo "Installing packages copied earlier from \"<-mkr_t_name_mixed-> Bonus CD 1\""
	cd $BonusWorkDir
	for PkgFile in $AutoInstPkg; do
		PkgName="${PkgFile%%_*}"
		echo "Installing '$PkgName'..."
		#dpkg -i "$PkgFile" &>/dev/null
		apt-get -f -y install "$PkgName"
	done
	# if monster-nucleus package is detected
	if [[ -n "$(echo monster-nucleus*)" ]]; then
		apt-get -f -y install monster-nucleus
		apt-get -f -y install bootsplash-theme-monster
	fi
	apt-get -f -y install id-my-disc || :
fi

#<-mkr_b_via_b->
apt-get -f -y install via-alsa-2.6.16.20-pluto-2-686 || :
#<-mkr_b_via_e->
