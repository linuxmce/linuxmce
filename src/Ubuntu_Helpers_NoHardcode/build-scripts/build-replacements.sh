#!/bin/bash -e

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/common/utils.sh

set -e

function Changed_Since_Last_Build
{
	return $(/bin/true) #Zaerc HACK

	local fs_path="$1"
	DisplayMessage "Checking for changes '$fs_path'"
	local cache_file="${replacements_dir}/.cache"
	local url_id=$(svn info "$fs_path" | grep '^URL: ' | cut -d' ' -f2 | md5sum | cut -d' ' -f1)
	local revision_new=$(svn info $fs_path | grep '^Revision: ' | cut -d' ' -f2)
	local revision_old=$(cat "$cache_file" | grep "^${url_id}" | cut -d'|' -f2)
	
	if [ "$revision_new" = "$revision_old" ] ; then
		return $(/bin/false)
	fi

	grep -v "$url_id" "$cache_file" > "${cache_file}.tmp"
	echo "$url_id|$revision_new" >> "${cache_file}.tmp"
	mv "${cache_file}.tmp" "$cache_file"

	return $(/bin/true)
}


function Build_Replacement_Package
{
	local pkg_name="$1"
	local pkg_dir="$2"


	dir_="${svn_dir}/${svn_branch_name}/${pkg_dir}"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building ${pkg_name}"
		pushd "$dir_"
		echo "dpkg-buildpackage -rfakeroot -us -uc -b -tc"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../${pkg_name}*.deb "${replacements_dir}"
		popd
	fi

}

function Build_Replacements_Common
{ 
	mkdir -pv "$replacements_dir"

	#Package: tee-pluto
	Build_Replacement_Package tee-pluto misc_utils/tee-pluto

	#Package: pluto-mplayer
	Build_Replacement_Package pluto-mplayer ubuntu/mplayer-svn26234

	#Package: pluto-ffmpeg
	Build_Replacement_Package pluto-ffmpeg ubuntu/ffmpeg-svn12476

	#Package: djmount
	Build_Replacement_Package djmount ubuntu/djmount-0.71

	#Package: freepbx
	Build_Replacement_Package freepbx ubuntu/asterisk/freepbx

        #Package: chan-sccp
	Build_Replacement_Package chan-sccp ubuntu/asterisk/chan_sccp

	#Package: pluto-asterisk
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/asterisk"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building pluto-asterisk"
		pushd "$dir_"
		./make_package_ubuntu.sh $KVER
		cp -r asterisk-pluto_*.deb ${replacements_dir}
		popd
	fi
###	Build_Replacement_Package asterisk-pluto ubuntu/asterisk

	#Package: lshwd
	Build_Replacement_Package lshwd_2.0 ubuntu/lshwd-2.0-rc4

	#Package: lirc-pluto
	Build_Replacement_Package lirc-pluto ubuntu/lirc-pluto-0.1

	#Package: mce-launcher
	Build_Replacement_Package mce-launcher src/mce-launcher

	#Package: mce-installer
	Build_Replacement_Package mce-installer src/mce-installer

	#Package: mtx-pluto
	Build_Replacement_Package mtx-pluto ubuntu/mtx-1.3.11

	#Package: parted
	Build_Replacement_Package parted ubuntu/parted-1.8.8
	cp ${svn_dir}/${svn_branch_name}/ubuntu/libparted*.deb ${replacements_dir}

	# TODO Fix this package so it builds against target kernel not running kernel
	#Package: linux-image-diskless
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/linux-image-dummy
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building linux-image-diskless for $KVER"
		pushd "$dir_"
		./makepackage.sh	
		cp linux-image-diskless_*.deb "${replacements_dir}"		
		popd
	fi

### Now done in prepare-scripts/import-external-files.sh, 
###  this here and src/pluto-avwizard-sounds can be removed
#	#Package: pluto-avwizard-sounds
#	Build_Replacement_Package pluto-avwizard-sounds src/pluto-avwizard-sounds

	#Package: asterisk-perl
	Build_Replacement_Package asterisk-perl external/asterisk-perl-0.10

	#Package: lmcevdr
	dir_="${svn_dir}/${svn_branch_name}"/ubuntu/lmcevdr-1.1
	if Changed_Since_Last_Build "$dir_" 
	then
		DisplayMessage "Building lmcevdr"
		pushd "$dir_"
		dpkg-deb -b . ..
		popd
	fi
	cp ${svn_dir}/${svn_branch_name}/ubuntu/lmcevdr_*.deb ${replacements_dir}

	#Package: vdr-plugin-control
	Build_Replacement_Package vdr-plugin-control ubuntu/vdr-plugin-control-0.0.2a+lmce

}

function Build_Replacements_Intrepid
{
	mkdir -pv "$replacements_dir"

#	#Package: lirc
#	Build_Replacement_Package lirc ubuntu/lirc-0.8.3+lmce
#	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-x*.deb ${replacements_dir}
#	cp ${svn_dir}/${svn_branch_name}/ubuntu/liblircclient{0,-dev}_*.deb ${replacements_dir}

	#Package: lirc
# NOTE: using "dpkg-buildpackage ... -tc" makes building lirc fail randomly
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/lirc-0.8.3+lmce"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building lirc"
		pushd "$dir_"
		echo "dpkg-buildpackage -rfakeroot -us -uc -b"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../*lirc*.deb "${replacements_dir}"
		popd
	fi

        #Package: libxine 	
#	Build_Replacement_Package libxine ubuntu/xine-lib-1.1.16.3-0ubuntu2~xine 
	Build_Replacement_Package libxine ubuntu/xine-lib-1.1.16.3-lmce0

        #Package: alsa dianemo packages

        wget http://ftp.sjtu.edu.cn/ubuntu/pool/main/libt/libtool/libtool_1.5.26-1ubuntu1_i386.deb
        dpkg -i libtool_1.5.26-1ubuntu1_i386.deb
        Build_Replacement_Package libasound2 ubuntu/alsa-lib-1.0.18
        # and now we go back to the latest libtool
        apt-get install libtool
	if ! dpkg -l libasound2-dev &>/dev/null; then
		dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/libasound2_*deb
		dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/lib32asound2_*deb
		dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/libasound2-dev_*deb
		dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/lib32asound2-dev_*deb
	fi
	Build_Replacement_Package libasound2-plugins ubuntu/alsa-plugins-1.0.18
	Build_Replacement_Package alsa-utils ubuntu/alsa-utils-1.0.18
	cp ${svn_dir}/${svn_branch_name}/ubuntu/*asound*deb ${replacements_dir}
	cp ${svn_dir}/${svn_branch_name}/ubuntu/alsa*deb ${replacements_dir}



	#Package: vdr-plugin-xineliboutput
	Build_Replacement_Package vdr-plugin-xineliboutput ubuntu/vdr-plugin-xineliboutput-1.0.1+lmce
	cp ${svn_dir}/${svn_branch_name}/ubuntu/libxine*.deb ${replacements_dir}
	cp ${svn_dir}/${svn_branch_name}/ubuntu/xineliboutput-*.deb ${replacements_dir}

	#Package: zaptel-modules
	DisplayMessage "Building zaptel-modules"
	m-a --non-inter -ft -l $KVER a-b zaptel
	cp /usr/src/zaptel-modules*.deb "${replacements_dir}"

	#Package: alsa-modules
	if [[ -f alsa-driver.tar.bz2 ]]; then
		DisplayMessage "Building alsa-modules from existing drivers archive"
		m-a --non-inter -t -l $KVER a-b alsa
		cp /usr/src/alsa-modules*.deb "${replacements_dir}"
	else
		DisplayMessage "Not building alsa-modules, as the drivers archive does not exist"
	fi
}

function Build_Replacements_Hardy
{
	mkdir -pv "$replacements_dir"

	#Package: mythtv
	Build_Replacement_Package mythtv ubuntu/mythtv-0.20.2+fixes14472
	cp ${svn_dir}/${svn_branch_name}/ubuntu/{libmyth,ubuntu-mythtv}*.deb ${replacements_dir}

	#Package: spcp8x5
	Build_Replacement_Package spcp8x5 ubuntu/spcp8x5
	# Included in Intrepid's linux-image.

	#Package: fuppes
	Build_Replacement_Package fuppes ubuntu/fuppes-0+svn578
	# Will be replaced by mediatomb in Intrepid

	#Package: zaptel-modules
	DisplayMessage "Building zaptel-modules"
	m-a --non-inter -ft -l $KVER a-b zaptel
	cp /usr/src/zaptel-modules*.deb "${replacements_dir}"

	#Package: alsa-modules
	DisplayMessage "Building alsa-modules"
	m-a --non-inter -ft -l $KVER a-b alsa
	cp /usr/src/alsa-modules-*.deb "${replacements_dir}"

	#Package: lirc
	Build_Replacement_Package lirc ubuntu/lirc-0.8.3~pre1+lmce
	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-x*.deb ${replacements_dir}
	cp ${svn_dir}/${svn_branch_name}/ubuntu/liblircclient{0,-dev}_*.deb ${replacements_dir}

	Build_Replacement_Package pluto-default-tftpboot src/pluto-default-tftpboot-2.0.0.44.0804
}

function Build_Replacements_Gutsy
{
	mkdir -pv "$replacements_dir"

	#Package: mythtv
	Build_Replacement_Package mythtv ubuntu/mythtv-0.20.2+fixes14472
	cp ${svn_dir}/${svn_branch_name}/ubuntu/{libmyth,ubuntu-mythtv}*.deb ${replacements_dir}

	#Package: libsdl
	Build_Replacement_Package libsdl1.2debian-pluto ubuntu/libsdl1.2-1.2.12

	#Package: spcp8x5
	Build_Replacement_Package spcp8x5 ubuntu/spcp8x5

	#Package: fuppes
	Build_Replacement_Package fuppes ubuntu/fuppes-0+svn578

	#Package: libxine
	Build_Replacement_Package libxine ubuntu/xine-lib-1.1.10.1

	#Package: zaptel-modules
	DisplayMessage "Building zaptel-modules"
	m-a --non-inter -ft -l $KVER a-b zaptel
	cp /usr/src/zaptel-modules*.deb "${replacements_dir}"

	#Package: alsa-modules
	DisplayMessage "Building alsa-modules"
	m-a --non-inter -ft -l $KVER a-b alsa
	cp /usr/src/alsa-modules-*.deb "${replacements_dir}"

	# DISABLED IN HARDY : not compiling anymore
	# Package: liblinphone
#	dir_="${svn_dir}/${svn_branch_name}/ubuntu/linphone-1.3.5"
#	if Changed_Since_Last_Build "$dir_" ;then
#		DisplayMessage "Building linphone-1.3.5"
#		pushd "$dir_"
#		dpkg-buildpackage -rfakeroot -us -uc -b -tc
###		cp ../liblinphone1-lmce_*.deb ${replacements_dir}
###		cp ../libortp4-lmce_*.deb ${replacements_dir}
##		cp ../linphone*_*.deb ${replacements_dir}
#		popd
#	fi
	Build_Replacement_Package linphone ubuntu/linphone-1.3.5
	cp ${svn_dir}/${svn_branch_name}/ubuntu/{liblinphone1-lmce_,libortp4-lmce_}*.deb ${replacements_dir}

	#Package: lirc
	Build_Replacement_Package lirc ubuntu/lirc-0.8.2+lmce
	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-x*.deb ${replacements_dir}
	cp ${svn_dir}/${svn_branch_name}/ubuntu/liblircclient{0,-dev}_*.deb ${replacements_dir}

	# DISABLED IN HARDY : included in kernel package
	#Package: lirc-modules
	pushd .
		DisplayMessage "Building lirc-modules"
                cd ${svn_dir}/${svn_branch_name}/src/Ubuntu_Helpers
                rm -f /etc/lirc/lirc-modules-source.conf
                dpkg-reconfigure -fnoninteractive lirc-modules-source

                mkdir -p /usr/src/modules/lirc/drivers/media/video/bt8xx/
                cp -a $KDIR/drivers/media/video/bt8xx/* /lib/modules/$KVER/build/drivers/media/video
                cp -a $KDIR/drivers/media/video/* /lib/modules/$KVER/build/drivers/media/video

                cd $KDIR/drivers/media/video
                cp -a btcx-risc.h /usr/src/modules/lirc/drivers/media/video
                m-a --non-inter -ft -l $KVER a-b lirc-modules
                cp /usr/src/lirc-modules*.deb "${replacements_dir}"
	popd

	# DISABLED IN HARDY : included in kernel package
	#Package: ivtv-modules
	pushd .
		DisplayMessage "Building ivtv-modules"
		m-a --non-inter -ft -l $KVER a-b ivtv
		cp /usr/src/ivtv-modules*.deb "${replacements_dir}"
	popd

	# DISABLED IN HARDY : included in kernel package
	#Package: ivtv-firmware
	pushd ${svn_dir}/${svn_branch_name}/ubuntu/
		DisplayMessage "Building ivtv-firmware"
		Src="deb http://dl.ivtvdriver.org/ubuntu feisty firmware"
		if [ ! -e /etc/apt/sources.list.pbackup ] ;then
			cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
		fi
		if ! grep -qF "$Src" /etc/apt/sources.list; then
			echo "$Src" >> /etc/apt/sources.list
			apt-get update
		fi

		rm -f ivtv-firmware_*.deb
		aptitude download ivtv-firmware
		mkdir ivtv-firmware
		cd ivtv-firmware
		dpkg -x ../ivtv-firmware_*.deb .
		dpkg -e ../ivtv-firmware_*.deb
		rm -f DEBIAN/preinst
		rm -f ../ivtv-firmware_*.deb
		dpkg-deb -b . ..
		cd ..
		rm -rf ivtv-firmware

		cp ivtv-firmware_*.deb "${replacements_dir}"
		rm ivtv-firmware_*.deb
	popd
}

trap 'Error "Undefined error in $0"' EXIT

DisplayMessage "*** STEP: Building replacement debs"
# make sure the headers are there
apt-get -y install linux-headers-$KVER
case "${build_name}" in
	"gutsy")
		Build_Replacements_Gutsy
		;;
	"hardy")
		Build_Replacements_Hardy
		;;
	"intrepid")
		Build_Replacements_Intrepid
		;;
esac
Build_Replacements_Common

DisplayMessage "Removing duplicate debs from replacements"
remove_duplicate_debs "${replacements_dir}"

trap - EXIT

