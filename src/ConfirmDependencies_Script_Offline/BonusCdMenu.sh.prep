#!/bin/bash

. /usr/pluto/install/Common.sh

# XXX: Setup Extras CD
echo "The <-mkr_t_name_mixed-> Extras CD version 1 has some extras, such as a video setup"
echo "wizard to help get you up and running. If you have the <-mkr_t_name_mixed-> Extras CD"
echo "version 1, please insert it into your drive now and choose Y after it is in."
echo "Otherwise, choose N."
echo ""
BonusCD=$(Ask "Did you insert the \"<-mkr_t_name_mixed-> Extras CD version 1\" in drive ? [Y/n]")
if [[ "$BonusCD" != N && "$BonusCD" != n ]]; then
	if [[ ! -d /media/cdrom ]]; then
		mkdir -p /media/cdrom
	fi
	/bin/mount -t iso9660 /dev/cdrom /media/cdrom 2>/dev/null

	count=0
	until [[ -d "/media/cdrom/bonuscd1" || "$counter" -eq 5 ]]; do
		((counter++))
		echo "This in not a valid \"<-mkr_t_name_mixed-> Extras CD version 1\". Please insert the correct CD and try again."
		/usr/bin/eject /media/cdrom &>/dev/null
		echo "Press any key when you inserted the correct CD in drive."
		read
		/bin/mount -t iso9660 /dev/cdrom /media/cdrom 2>/dev/null
	done
	if [[ "$counter" -eq 5 && ! -d /media/cdrom/bonuscd1 ]]; then
		echo "Skipping BonusCD due to invalid data"
		/usr/bin/eject /media/cdrom &>/dev/null
	fi
	echo "Processing \"<-mkr_t_name_mixed-> Extras CD version 1\""
	echo "... PLEASE WAIT ..."

	BonusWorkDir="/usr/pluto/deb-cache/dists/sarge/main/binary-i386"
	
	cd /media/cdrom/bonuscd1
	AutoInstPkg="$(echo *.deb)"
	if ! cp -r $AutoInstPkg "$BonusWorkDir"; then
		echo "ERROR: Failed copying first batch"
		read
	fi

	cd /media/cdrom/bonuscd1-cache
	if ! cp -r *.deb "$BonusWorkDir"; then
		echo "ERROR: Failed copying second batch"
		read
	fi

#<-mkr_b_via_b->
	cd /media/cdrom/via
	mkdir /home/via
	cp * /home/via
#<-mkr_b_via_e->

	sleep 1
	cd 
	/bin/umount /media/cdrom &>/dev/null
	/usr/bin/eject /dev/cdrom &>/dev/null
	echo ""
	echo "Finished processing \"<-mkr_t_name_mixed-> Extras CD version 1\""
	echo ""
	cd "$BonusWorkDir"
	# install dependencies for dpkg-scanpackages on offline installation
	dpkg -i libsepol1_*.deb 1>/dev/null 2>/dev/null
	dpkg -i libselinux1_*.deb 1>/dev/null 2>/dev/null
	dpkg -i binutils_*.deb 1>/dev/null 2>/dev/null
	dpkg -i patch_*.deb 1>/dev/null 2>/dev/null
	dpkg -i perl-modules_*.deb perl_*.deb 1>/dev/null 2>/dev/null
	dpkg -i dpkg_*.deb 1>/dev/null 2>/dev/null
	dpkg -i dpkg-dev_*.deb 1>/dev/null 2>/dev/null

	cd "$BonusWorkDir"
	if [ -f /usr/bin/dpkg-scanpackages ]; then
	# regenerate packages.gz
	echo "Regenerating Packages.gz in debcache. This will require 1 minute. Please wait."
	dpkg-scanpackages . /dev/null | sed 's,\./,dists/sarge/main/binary-i386/,g' | gzip -9c > Packages.gz
	clear
	# update apt cache
	echo "Updating apt cache files ..."
	apt-get update 1>/dev/null 2>/dev/null
	# install bonuscd packages
#	echo "Processing video-wizard-videos package ..."
#	dpkg -i video-wizard-videos-*.deb 1>/dev/null 2>/dev/null
	# if bonuscd monster was detected 
#	cd $BonusWorkDir
#	if [[ -f $(ls | grep monster-nucleus) && ! -z $(ls | grep monster-nucleus) ]]; then
#	apt-get -f -y install monster-nucleus 
#	apt-get -f -y install bootsplash-theme-monster
#	fi
	else
	echo "Command not found : dpkg-scanpackages. Packages.gz regeneration skipped."
	fi
else
	echo "Skipping \"<-mkr_t_name_mixed-> Extras CD version 1\"..."
fi
