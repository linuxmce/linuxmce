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
		dpkg-buildpackage -rfakeroot -us -uc -b 
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
#	Build_Replacement_Package vdr-plugin-control ubuntu/vdr-plugin-control-0.0.2a+lmce

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
	# Latest xine version
#	Build_Replacement_Package libxine ubuntu/xine-lib-1.1.17
	# niz version
#	Build_Replacement_Package libxine ubuntu/xine-lib-1.1.16.3-0ubuntu2~xine 
#	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/libxine1_*.deb \
#			    ${svn_dir}/${svn_branch_name}/ubuntu/libxine1-plugins_*.deb \
#			    ${svn_dir}/${svn_branch_name}/ubuntu/libxine1-x_*.deb \
#			    ${svn_dir}/${svn_branch_name}/ubuntu/libxine1-console_*.deb \
#			    ${svn_dir}/${svn_branch_name}/ubuntu/libxine1-bin_*.deb \
#			    ${svn_dir}/${svn_branch_name}/ubuntu/libxine1-misc-plugins_*.deb \
#			    ${svn_dir}/${svn_branch_name}/ubuntu/libxine1-ffmpeg_*.deb \
#			    ${svn_dir}/${svn_branch_name}/ubuntu/libxine-dev_*.deb \
#			    ${svn_dir}/${svn_branch_name}/ubuntu/libxine1-plugins_*.deb
#

	# Original version
	Build_Replacement_Package libxine1 ubuntu/xine-lib-1.1.16.4
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/libxine1_*.deb ${svn_dir}/${svn_branch_name}/ubuntu/libxine-dev_*.deb 
	# alsa lib needs older libtool package
        if [ "$MACHTYPE" = "x86_64-pc-linux-gnu" ]; then
        	wget http://ftp.sjtu.edu.cn/ubuntu/pool/main/libt/libtool/libtool_1.5.26-1ubuntu1_amd64.deb -O libtool_1.5.26-1ubuntu1_amd64.deb
                dpkg -i libtool_1.5.26-1ubuntu1_amd64.deb
        else
        	wget http://ftp.sjtu.edu.cn/ubuntu/pool/main/libt/libtool/libtool_1.5.26-1ubuntu1_i386.deb -O libtool_1.5.26-1ubuntu1_i386.deb
                dpkg -i libtool_1.5.26-1ubuntu1_i386.deb
        fi
	                                                                                        
        Build_Replacement_Package libasound2 ubuntu/alsa-lib-1.0.18
        # and now we go back to the latest libtool
        apt-get install libtool
	dpkg --force-all -i ${svn_dir}/${svn_branch_name}/ubuntu/libasound2_*deb ${svn_dir}/${svn_branch_name}/ubuntu/lib64asound2_*deb ${svn_dir}/${svn_branch_name}/ubuntu/libasound2-dev_*deb ${svn_dir}/${svn_branch_name}/ubuntu/lib64asound2-dev_*deb
	Build_Replacement_Package libasound2-plugins ubuntu/alsa-plugins-1.0.18
	Build_Replacement_Package alsa-utils ubuntu/alsa-utils-1.0.18
	cp ${svn_dir}/${svn_branch_name}/ubuntu/*asound*deb ${replacements_dir}
	cp ${svn_dir}/${svn_branch_name}/ubuntu/alsa*deb ${replacements_dir}

	#v4l-modules-source
	Build_Replacement_Package v4l2 ubuntu/v4l-modules-source

	#Package: mythtv
	# Remove packages that conflict with mythtv .23 build
        apt-get remove -y libmyth-dev qt3-dev-tools libqt3-mt-dev libartsc0-dev 
        Build_Replacement_Package mythtv ubuntu/mythtv-0.23
	# The dev package needs to be installed to build mythtv plugins.
	dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/libmyth-0.23-0*.deb
	dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/libmyth-dev_0.23-0*.deb
	Build_Replacement_Package mythplugins ubuntu/mythplugins-0.23
	Build_Replacement_Package mythtv-themes ubuntu/myththemes-0.23
        # Reinstall package needed for SimplePhone (package 498,499)
        apt-get install -y liblinphone2-dev 
	cp ${svn_dir}/${svn_branch_name}/ubuntu/{libmyth,myth,python}*.deb ${replacements_dir}

	# VDR Packages
	# When adding new VDR versions, and there is a need to modify the patches,
	# these are the steps to be taken:
	# 1) copy debian/patches/00list (or 00list.extensions) to debian/patches.00list.mypatchvariant
	# 2) enable the patches you would like to use in debian/patches.00list.mypatchvariant by uncommenting/commenting them (keep in mind, that some patches depend on each other and therefore some combination might lead to rejects, that you have to resolve yourself)
	# 3) PATCHVARIANT=mypatchvariant debian/rules accept-patches
	# 4) PATCHVARIANT=mypatchvariant dpkg-buildpackage -tc -uc -us -rfakeroot
	#   or
	# 4) export PATCHVARIANT=mypatchvariant and dpkg-buildpackge
	# 5) install vdr-dev (this is done in this script)
	# 6) build ALL the plug-ins you would like to use (this is done in this script)
	
	DisplayMessage "Building VDR packages and plugins"
	export PATCHVARIANT=multipatch
	Build_Replacement_Package vdr ubuntu/vdr-1.7.15
	# To build the plugins, we need to have the current -dev package installed
	dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/vdr-dev_*.deb
	
	Build_Replacement_Package vdr-plugin-control ubuntu/vdr-plugin-control-0.0.2a
	Build_Replacement_Package vdr-plugin-svdrpservice ubuntu/vdr-plugin-svdrpservice-0.0.4
	# Before installing svdrservce-dev vdr needs to be installed
	dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/vdr_*.deb
	/etc/init.d/vdr stop
	# The dev package needs to be installed to build the remote timers plugin.
	
        dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/svdrpservice-dev_*.deb
	Build_Replacement_Package vdr-plugin-remotetimers ubuntu/vdr-plugin-remotetimers-0.1.3
	Build_Replacement_Package vdr-plugin-streamdev ubuntu/vdr-plugin-streamdev-0.5.0~pre20090706+cvs20091108.2341
#	Build_Replacement_Package vdr-plugin-streamdev ubuntu/vdr-plugin-streamdev-0.5.0~pre20090706+cvs20100307.2102
	Build_Replacement_Package vdr-plugin-vompserver ubuntu/vdr-plugin-vompserver-0.3.1
	Build_Replacement_Package vdr-addon-noad ubuntu/vdr-addon-noad-0.7.2
#	Build_Replacement_Package vdr-plugin-xineliboutput ubuntu/vdr-plugin-xineliboutput-1.0.4+cvs20091215.2049
	Build_Replacement_Package vdr-plugin-xineliboutput ubuntu/vdr-plugin-xineliboutput-1.0.6+cvs20100608.2009
	Build_Replacement_Package vdr-plugin-wirbelscan ubuntu/vdr-plugin-wirbelscan-0.0.5~pre12a
	Build_Replacement_Package vdr-plugin-iptv ubuntu/vdr-plugin-iptv-0.4.2

	cp ${svn_dir}/${svn_branch_name}/ubuntu/libxine*.deb ${replacements_dir}
	cp ${svn_dir}/${svn_branch_name}/ubuntu/xineliboutput-*.deb ${replacements_dir}

	# SqueezeSlave
	Build_Replacement_Package squeezeslave ubuntu/squeezeslave


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

trap 'Error "Undefined error in $0" ; apt-get install libtool -y' EXIT

DisplayMessage "*** STEP: Building replacement debs"
# make sure the headers are there
apt-get --force-yes -y install linux-headers-$KVER 
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

