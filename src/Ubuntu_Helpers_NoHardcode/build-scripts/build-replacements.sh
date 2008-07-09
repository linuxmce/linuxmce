#!/bin/bash -e

. /usr/local/lmce-build/common/env.sh
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/common/utils.sh

set -e
set -x

function Changed_Since_Last_Build {
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

function Build_Replacements_Common { 
	mkdir -p "$replacements_dir"

	build_zaptel="yes"
        build_mce_installer="yes"
        build_alsa_modules="yes"
	case "$(lsb_release -c -s)" in
		"gutsy")
		;;
		"hardy")
		;;
		"intrepid")
		build_zaptel="no"
                build_mce_installer="no"
	        build_alsa_modules="no"
		;;
	esac


	#Package: libsdl
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/libsdl1.2-1.2.12"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building libsdl1.2debian-pluto"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../libsdl1.2debian-pluto*.deb "${replacements_dir}"
		popd
	fi

	#Package: spcp8x5
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/spcp8x5"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building spcp8x5"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../spcp8x5*.deb ${replacements_dir}
		popd
	fi

	#Package: libxine
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/xine-lib-1.1.10.1"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building xine-lib"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../libxine*.deb ${replacements_dir}
		popd
	fi

	#Package: mythtv
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/mythtv-0.20.2+fixes14472"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building mythtv"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../libmyth*.deb ${replacements_dir}
		cp -r ../mythtv*.deb ${replacements_dir}
		cp -r ../ubuntu-mythtv*.deb ${replacements_dir}
		popd
	fi

	#Package: tee-pluto
	dir_="${svn_dir}/${svn_branch_name}/misc_utils/tee-pluto"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building tee-pluto"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../tee-pluto_*.deb ${replacements_dir}
		popd
	fi


	#Package: pluto-mplayer
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/mplayer-svn26234"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building pluto-mplayer-svn26234"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../pluto-mplayer_*.deb ${replacements_dir}
		popd
	fi
	
	#Package: pluto-ffmpeg
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/ffmpeg-svn12476"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building pluto-ffmpeg"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../pluto-ffmpeg_*.deb ${replacements_dir}
		popd
	fi
	
	#Package: fuppes
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/fuppes-0+svn578"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building fuppes-0+svn578"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../fuppes_*.deb ${replacements_dir}
		popd
	fi
	
	#Package: djmount
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/djmount-0.71"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building djmount-0.71"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../djmount_*.deb ${replacements_dir}
		popd
	fi

	#Package: freepbx
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/asterisk/freepbx"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building freepbx"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -r ../freepbx_*.deb ${replacements_dir}
		popd
	fi

        #Package: chan-sccp
        dir_="${svn_dir}/${svn_branch_name}/ubuntu/asterisk/chan_sccp"
        if ReplacementNeedsBuild "$dir_"; then
		DisplayMessage "Build chan-sccp"
                pushd "$dir_"
                dpkg-buildpackage -rfakeroot -us -uc -b -tc
                cp -r ../chan-sccp_*.deb ${replacements_dir}
                popd
        fi

	#Package: pluto-asterisk
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/asterisk"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building pluto-asterisk"
		pushd "$dir_"
		./make_package_ubuntu.sh $KVER
		cp -r asterisk-pluto_*.deb ${replacements_dir}
		popd
	fi

	#Package: lshwd
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/lshwd-2.0-rc4"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building lshwd-2.0-rc4"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../lshwd_2.0*.deb ${replacements_dir}
		popd
	fi

        if [ x"$build_zaptel" = x"yes" ] ; then
		#Package: zaptel-modules
		DisplayMessage "Building zaptel-modules"
		m-a --non-inter -ft -l $KVER a-b zaptel
		cp /usr/src/zaptel-modules*.deb "${replacements_dir}"
	fi

	#Package: lirc-pluto
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/lirc-pluto-0.1
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building lirc-pluto"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../lirc-pluto_*.deb "${replacements_dir}"
		popd
	fi

	#Package: lirc
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/lirc-0.8.2+lmce
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../lirc{,-x}_*.deb ../liblircclient{0,-dev}_*.deb "${replacements_dir}"
		popd
	fi

	
	#Package: mce-launcher
	dir_=${svn_dir}/${svn_branch_name}/src/mce-launcher
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building mce-launcher"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../mce-launcher_*.deb "${replacements_dir}"
		popd
	fi

	#Package: mce-installer
        if [ x"$build_mce_installer" = x"yes" ] ; then
		dir_=${svn_dir}/${svn_branch_name}/src/mce-installer
		if Changed_Since_Last_Build "$dir_" ;then
			DisplayMessage "Building mce-installer"
			pushd "$dir_"
			dpkg-buildpackage -rfakeroot -us -uc -b -tc
			cp ../mce-installer_*.deb "${replacements_dir}"
			popd
		fi
	fi

	#Package: mtx-pluto
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/mtx-1.3.11
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building mtx-pluto"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../mtx-pluto_*.deb "${replacements_dir}"
		popd
	fi

	#Package: parted
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/parted-1.8.8
	if Changed_Since_Last_Build "$dir_"; then
		DisplayMessage "Building parted"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../*parted*.deb "${replacements_dir}"
	fi

	# TODO Fix this package so it builds against target kernel not running kernel
	#Package: linux-image-diskless
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/linux-image-dummy
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building linux-image-diskless"
		pushd "$dir_"
		./makepackage.sh	
		cp linux-image-diskless_*.deb "${replacements_dir}"		
		popd
	fi

	if [ x"$build_alsa_modules" = x"yes" ] ; then
		#Package: alsa-modules
		DisplayMessage "Building alsa-modules"
		m-a --non-inter -ft -l $KVER a-b alsa
		cp /usr/src/alsa-modules-*.deb "${replacements_dir}"
	fi

	#Package: pluto-avwizard-sounds
	dir_=${svn_dir}/${svn_branch_name}/src/pluto-avwizard-sounds
	if Changed_Since_Last_Build "$dir_" ;then
		pushd $dir_
			DisplayMessage "Building av-wizard-sounds"
			dpkg-buildpackage -rfakeroot -us -uc -b -tc
			cp ../pluto-avwizard-sounds_*.deb "${replacements_dir}"
		popd
	fi
}

function Build_Replacements_Hardy {
	#Package: chan-sccp
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/asterisk/chan_sccp"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building chan-sccp"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../chan-sccp_*.deb ${replacements_dir}
		popd
	fi
}

function Build_Replacements_Gutsy {
	mkdir -p "$replacements_dir"

	# DISABLED IN HARDY : not compiling anymore
	# Package: liblinphone
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/linphone-1.3.5"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building linphone-1.3.5"
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp ../liblinphone1-lmce_*.deb ${replacements_dir}
		cp ../libortp4-lmce_*.deb ${replacements_dir}
		cp ../linphone*_*.deb ${replacements_dir}
		popd
	fi

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
Build_Replacements_Common

case "$(lsb_release -c -s)" in
	"gutsy")
		Build_Replacements_Gutsy
		;;
	"hardy")
		Build_Replacements_Hardy
		;;
esac

DisplayMessage "Removing duplicate debs from replacements"
remove_duplicate_debs "${replacements_dir}"

trap - EXIT
