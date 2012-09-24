#!/bin/bash -x

. /usr/pluto/bin/Utils.sh

export KDE_LMCE=""

## Log every message that the builder outputs in the logfile
exec 100>&1
#exec >/var/log/BuildUbuntu.log
#exec 2>&1

flavor="ubuntu"

if [[ $build_dir == "" || $local_mirror_dir == "" ]] ;then
	build_dir="/var/plutobuild"
	local_mirror_dir="/var/www"
fi

svn_dir="${build_dir}/svn"
svn_url="http://svn.linuxmce.com/"
svn_private_url="http://svn.plutohome.com/"

sql_master_host="sqlcvs.plutohome.com"
sql_master_db="pluto_main"
sql_master_db_media="pluto_media"
sql_master_db_security="pluto_security"
sql_master_db_telecom="pluto_telecom"
sql_master_user="builder"

sql_slave_host="127.0.0.1"
sql_slave_db="pluto_main_build"
sql_slave_db_mainsqlcvs="main_sqlcvs_$flavor"
sql_slave_db_mythsqlcvs="myth_sqlcvs_$flavor"
sql_slave_db_media="pluto_media"
sql_slave_db_security="pluto_security"
sql_slave_db_telecom="pluto_telecom"
sql_slave_user="root"

replacements_dir="${build_dir}/replacements"
out_dir="${build_dir}/out"
mkr_dir="${build_dir}/MakeRelease"
priv_debs_dir="$build_dir/private_debs"

if [[ "$iso_name" == "" ]] ;then
	iso_name="linuxmce-1.1"
fi

function DisplayMessage {
	echo "`date +%H:%M:%S`  $*" >&100
}

function Error {
	echo
	DisplayMessage "ERROR: $*"
	tail -20 /var/log/BuildUbuntu.log
	exit 1
}


function WaitDisklessImage {

	local arch="$1"
	local done='false'

	local RemoteVersion=""
	local ExpectedVersion="$(echo "select VersionName from Version" | mysql -A -N pluto_main_build  | head -1)"

	while [[ "$done" != 'true' ]] ;do
		RemoteVersion=$(cat "/var/plutobuild/DisklessSync/${arch}/version")
		
		if [[ "$RemoteVersion" == "$ExpectedVersion" ]] ;then
			done='true'
		else
			sleep 30
		fi	
	done
}

function CopyCrossoverDebcache {

	local arch="$1"
	local sync_pkgs="pluto-bluetooth-dongle pluto-cm11a pluto-eib pluto-gc100 pluto-generic-serial-device pluto-hdhomerun pluto-irbase pluto-irtrans-wrapper pluto-libbd pluto-lirc-wrapper pluto-messagetrans pluto-motion-wrapper pluto-msiml-disp-butt pluto-nvidia-video-drivers pluto-powerfile-c200 pluto-proxy-orbiter pluto-text-to-speech pluto-tira-wrapper pluto-usb-uirt-0038 pluto-vdr pluto-vipshared pluto-wavetrend-reader pluto-zwave-lighting mtx-pluto lirc-pluto lirc-modules-2.6.22-14-generic id-my-disc linux-restricted-modules-2.6.22-14-generic"


	local pkg
	for pkg in $sync_pkgs ;do
		rm -f "/var/plutobuild/DisklessSync/${arch}/deb-cache/${pkg}_"*.deb
		cp "/var/www/${pkg}_"*.deb /var/plutobuild/DisklessSync/${arch}/deb-cache/
	done
}

function Install_Build_Needed_Packages {
	#TODO:Get list outside the file as it started growing to big
	local pkgs="asterisk-dev squashfs-tools libavcodec-dev libavformat-dev libpostproc-dev libcaca-dev libmodplug-dev libtheora-dev libgnomevfs2-dev libflac-dev libsmbclient-dev libmad0-dev libcdio-dev transfig sgmltools-lite automake nasm x-dev libsvga1-dev dmsetup mkisofs libxcb-xv0-dev libxcb-shm0-dev libxcb-shape0-dev libaa1-dev libmagick9-dev libdirectfb-dev libpulse-dev libmpcdec-dev kdelibs4-dev qt3-qtconfig qt3-linguist qt3-dev-tools-embedded qt3-dev-tools-compat qt3-dev-tools qt3-designer qt3-assistant qt3-apps-dev libqt3-mt-dev libqt3-headers libqt3-compat-headers xorg-dev qt3-dev-tools libxcb-xv0-dev libxcb-shm0-dev libxcb-shape0-dev libmagick9-dev libmpcdec-dev quilt tcl8.4-dev libfuse-dev fuse-utils libupnp-dev libconfuse-dev subversion build-essential dh-make libmysqlclient15-dev libhttpfetcher-dev libattr1-dev libdbus-1-dev libdbus-glib-1-dev libhal-dev libdancer-xml0-dev libbluetooth2-dev libid3-3.8.3-dev libxine-dev x11proto-core-dev libx11-dev libx11-dev x11proto-core-dev x11proto-xext-dev x11proto-xf86vidmode-dev libx11-dev libjpeg62-dev libcdparanoia0-dev libsdl1.2-dev libsdl-gfx1.2-dev libxmu-headers x11proto-record-dev libhid-dev libusb-dev libsdl-image1.2-dev libsdl-ttf2.0-dev libsdl-sge-dev libxtst-dev libxrender-dev libcddb-dev libdvdread-dev libcurl3-dev ruby1.8-dev swig libtcltk-ruby mysql-client mysql-server libmediastreamer0-dev libgtk2.0-dev libvte-dev libglade2-dev libstdc++5 liblircclient-dev fakeroot" 
	local pkg
	for pkg in $pkgs ;do
		apt-get -y install $pkg
	done
}

function Checkout_Pluto_Svn {
	DisplayMessage "**** STEP : SVN CHECKOUT"
	local Branch="${1:-trunk}"
	local BranchParts="$2" # ex: "src web"; the rest will come from trunk

	local AllParts="src ubuntu web misc_utils installers config-pkgs"
	if [[ -z "$BranchParts" ]]; then
		BranchParts="$AllParts"
	fi

	DisplayMessage "Removing old svn checkout dir"
	[[ -d $svn_dir ]] && mkdir -p $svn_dir
	rm -rf ${svn_dir}/trunk
	
	for svn_module in ${BranchParts}; do
		mkdir -p ${svn_dir}/trunk/$svn_module
		DisplayMessage "Checking out {$svn_url}/pluto/$Branch/$svn_module"
		svn co  ${svn_url}/pluto/"$Branch"/$svn_module  ${svn_dir}/trunk/$svn_module || Error "Failed to checkout {$svn_url}/pluto/$Branch/$svn_module"
	done

	# get unmarked parts from trunk
	for svn_module in ${AllParts}; do
		if [[ " $BranchParts " == *" $svn_module "* ]]; then
			# this part was marked to be taken from the branch
			continue
		fi

		#get part from trunk
		mkdir -p ${svn_dir}/trunk/$svn_module
		DisplayMessage "Checking out {$svn_url}/pluto/trunk/$svn_module"
		svn co ${svn_url}/pluto/trunk/$svn_module  ${svn_dir}/trunk/$svn_module || Error "Failed to checkout {$svn_url}/pluto/trunk/$svn_module"
	done


	pushd ${svn_dir}/trunk/src
	DisplayMessage "Checking out {$svn_url}/pluto-private/$Branch/"
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_private_url}/pluto-private/"$Branch"/src/ZWave/ || Error "Failed to checkount {$svn_url}/pluto-private/$Branch/src/ZWave"
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_private_url}/pluto-private/"$Branch"/src/Fiire_Scripts/ || Error "Failed to checkount {$svn_url}/pluto-private/$Branch/src/Fiire_Scripts"
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_private_url}/pluto-private/"$Branch"/src/RFID_Interface/ || Error "Failed to checkount {$svn_url}/pluto-private/$Branch/src/RFID_Interface"
#	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_private_url}/pluto-private/"$Branch"/src/lmce_launch_manager/ || Error "Failed to checkount {$svn_url}/pluto-private/$Branch/src/lmce_launch_manager"
	popd
}


function Build_MakeRelease_Binary {
	DisplayMessage "**** STEP : PREPARING BUILD SYSTEM (MakeRelase)"
	DisplayMessage "Precompiling pluto_main"
	pushd ${svn_dir}/trunk/src/pluto_main
	make || Error "Failed to precompile pluto_main to use for MakeRelease"
	popd

	DisplayMessage "Precompiling PlutoUtils"
	pushd ${svn_dir}/trunk/src/PlutoUtils
	make || Error "Failed to precompile PlutoUtils"
	popd

	DisplayMessage "Precompiling SerializeClass"
	pushd ${svn_dir}/trunk/src/SerializeClass
	SNR_CPPFLAGS="" make || Error "Failed to precompile SerializeClass"
	popd

	DisplayMessage "Precompiling LibDCE"
	pushd ${svn_dir}/trunk/src/DCE
	SNR_CPPFLAGS="" make || Error "Failed to precompile LibDCE"
	popd

	DisplayMessage "Precompiling MakeRelease"
	pushd ${svn_dir}/trunk/src/MakeRelease
	make -f Makefile.MakeRelease || Error "Failed to precompile MakeRelease"
	popd

	DisplayMessage "Precompiling MakeRelease_PrepFiles"
	pushd ${svn_dir}/trunk/src/MakeRelease_PrepFiles
	make || Error "Failed to precompile MakeRelease_PrepFiles"
	popd

	DisplayMessage "Precompiling mysql_wrapper"
	pushd ${svn_dir}/trunk/src/mysql_wrapper
	make || Error "Failed to precompile mysql_wrapper"
	popd

	mkdir -p ${mkr_dir}
	mkdir -p /usr/pluto/bin
	cp ${svn_dir}/trunk/src/bin/MakeRelease ${mkr_dir}
	cp ${svn_dir}/trunk/src/bin/MakeRelease_PrepFiles ${mkr_dir}
	cp ${svn_dir}/trunk/src/bin/mysql_wrapper /usr/pluto/bin/
	cp ${svn_dir}/trunk/src/lib/*.so ${mkr_dir}
}


function ReplacementNeedsBuild {
	local fs_path="$1"
	local cache_file="/var/plutobuild/replacements/.cache"

	local url_id=$(svn info $fs_path | grep '^URL: ' | cut -d' ' -f2 | md5sum | cut -d' ' -f1)

	local revision_new=$(svn info $fs_path | grep '^Revision: ' | cut -d' ' -f2)
	local revision_old=$(cat "$cache_file" | grep "^${url_id}" | cut -d'|' -f2)
	
	if [[ "$revision_new" == "$revision_old" ]] ;then
		return $(/bin/false)
	fi

	grep -v "$url_id" "$cache_file" > "${cache_file}.tmp"
	echo "$url_id|$revision_new" >> "${cache_file}.tmp"
	mv "${cache_file}.tmp" "$cache_file"

	return $(/bin/true)
}


function Build_Pluto_Replacements { 
	temp_dir="${replacements_dir}"
	mkdir -p $temp_dir

	#Package: libsdl
	dir_="${svn_dir}/trunk/ubuntu/libsdl1.2-1.2.12"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../libsdl1.2debian-pluto*.deb ${temp_dir}
		popd
	fi

	#Package: libxine
	dir_="${svn_dir}/trunk/ubuntu/xine-lib-1.1.7"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../libxine*.deb ${temp_dir}
		popd
	fi

	#Package: video-wizard-videos
	local vvv_temp_dir=$(mktemp -d)
	pushd "$vvv_temp_dir"
		scp -r pluto@10.0.2.4:"/home/samba/www_docs/video\ wizard/video-wizard-videos" ./
		cd "video-wizard-videos"
		dpkg-deb -b . ..
		cp -r ../video-wizard-videos_*.deb ${temp_dir}
	popd
	rm -rf "$vvv_temp_dir"

	#Package: tee-pluto
	dir_="${svn_dir}/trunk/misc_utils/tee-pluto"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../tee-pluto_*.deb ${temp_dir}
		popd
	fi
	
	#Package: liblinphone
	dir_="${svn_dir}/trunk/ubuntu/linphone-1.3.5"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../liblinphone1-lmce_*.deb ${temp_dir}
		cp ../libortp4-lmce_*.deb ${temp_dir}
		popd
	fi

	#Package: pluto-mplayer
	dir_="${svn_dir}/trunk/ubuntu/mplayer"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../pluto-mplayer_*.deb ${temp_dir}
		popd
	fi
	
	#Package: pluto-ffmpeg
	dir_="${svn_dir}/trunk/ubuntu/ffmpeg"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../pluto-ffmpeg_*.deb ${temp_dir}
		popd
	fi
	
	#Package: ushare
	dir_="${svn_dir}/trunk/ubuntu/ushare-0.9.6"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../ushare_*.deb ${temp_dir}
		popd
	fi
	
	#Package: djmount
	dir_="${svn_dir}/trunk/ubuntu/djmount-0.71"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../djmount_*.deb ${temp_dir}
		popd
	fi

	#Package: freepbx
	dir_="${svn_dir}/trunk/ubuntu/asterisk/freepbx"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../freepbx_*.deb ${temp_dir}
		popd
	fi

	#Package: chan-sccp
	dir_="${snv_dir}/trunk/ubuntu/asterisk/chan_sccp"
	if ReplacementNeedsBuild "$dir_"; then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../chan-sccp_*.deb ${temp_dir}
		popd
	fi

	#Package: pluto-asterisk
	apt-get -y install linux-headers-`uname -r`
	dir_="${svn_dir}/trunk/ubuntu/asterisk"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		./make_package_ubuntu.sh `uname -r`
		cp -r asterisk-pluto_*.deb ${temp_dir}
		popd
	fi

	#Package: lshwd
	dir_="${svn_dir}/trunk/ubuntu/lshwd-2.0-rc4"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../lshwd_2.0*.deb ${temp_dir}
		popd
	fi

	#Package: lirc-modules
	apt-get -y install linux-source-`uname -r | cut -d'-' -f1` linux-headers-`uname -r` module-assistant lirc-modules-source
	pushd .
                cd "${svn_dir}"/trunk/src/Ubuntu_Helpers
                ./Preseed_lirc-modules-source.sh
                rm -f /etc/lirc/lirc-modules-source.conf
                dpkg-reconfigure -fnoninteractive lirc-modules-source

		pushd /usr/src
			tar jxvf linux-source-`uname -r | cut -d '-' -f1`.tar.bz2
		popd

                mkdir -p /usr/src/modules/lirc/drivers/media/video/bt8xx/
                cp -a /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video/bt8xx/* /lib/modules/`uname -r`/build/drivers/media/video
                cp -a /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video/* /lib/modules/`uname -r`/build/drivers/media/video

                cd /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video
                cp -a btcx-risc.h /usr/src/modules/lirc/drivers/media/video
                cd /usr/src/modules/lirc/drivers
                ln -s ../../../linux-source-2.6.17/drivers/video/ ./  || :
                m-a -ft a-b lirc-modules
                cp /usr/src/lirc-modules*.deb "${temp_dir}"
	popd
	
	#Package: zaptel-modules
	apt-get -y install zaptel-source
	m-a -ft -l 2.6.22-14-generic a-b zaptel
	cp /usr/src/zaptel-modules*.deb "${temp_dir}"

	#Package: ivtv-modules
	apt-get -y install ivtv-source
	pushd .
		m-a -ft a-b ivtv
		cp /usr/src/ivtv-modules*.deb "${temp_dir}"
	popd
	
	pushd "${svn_dir}"/trunk/ubuntu/
		Src="deb http://dl.ivtvdriver.org/ubuntu feisty firmware"
		if [ ! -e /etc/apt/sources.list.pbackup ] ;then
			cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
		fi
		if ! grep -qF "$Src" /etc/apt/sources.list; then
			echo "$Src" >> /etc/apt/sources.list
			apt-get update
		fi

		/root/Ubuntu_Helpers/RepackIvtvFirmware.sh

		cp ivtv-firmware_*.deb "${temp_dir}"
		rm ivtv-firmware_*.deb
	popd

	#Package: lirc-pluto
	dir_="${svn_dir}"/trunk/ubuntu/lirc-pluto-0.1
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../lirc-pluto_*.deb "${temp_dir}"
		popd
	fi
	
	#Package: lirc
	dir_="${svn_dir}"/trunk/ubuntu/lirc-0.8.2+lmce
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../lirc{,-x}_*.deb ../liblircclient{0,-dev}_*.deb "${temp_dir}"
		popd
	fi
	
	#Package: mce-launcher
	dir_="${svn_dir}"/trunk/src/mce-launcher
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mce-launcher_*.deb "${temp_dir}"
		popd
	fi

	#Package: mce-installer
	dir_="${svn_dir}"/trunk/src/mce-installer
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mce-installer_*.deb "${temp_dir}"
		popd
	fi

	#Package: mtx-pluto
	dir_="${svn_dir}"/trunk/ubuntu/mtx-1.3.11
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../mtx-pluto_*.deb "${temp_dir}"
		popd
	fi

	#Package: linux-image-dummy
	dir_="${svn_dir}"/trunk/ubuntu/linux-image-dummy
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		./makepackage.sh	
		cp linux-image-diskless_*.deb "${temp_dir}"		
		popd
	fi

	#Download arch independent packages from 150
	pushd $temp_dir
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/libflickr-api-perl_1_all.deb ./
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/libxml-parser-lite-tree-perl_1_all.deb ./
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/asterisk-perl_0.08-1_all.deb ./
#		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/pluto-avwizard-sounds_1.0-1_all.deb ./
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/msttcorefonts_2.2pluto1_all.deb ./
	popd 

}

function Build_Pluto_Stuff_Compile {
	:
}

function Build_Pluto_Stuff_Debs {
	:
}

function Build_Pluto_Stuff {
	MakeReleaseExtraParams=("$@")

	
	touch /home/README.Devel.Dependencies
	mkdir -p /usr/pluto/dummy-packages/
	touch /usr/pluto/dummy-packages/Readme.PlutoConsoleUtilities.dummy

	export PATH=$PATH:${svn_dir}/trunk/src/bin
#	export PATH=/usr/lib/ccache:$PATH:${svn_dir}/trunk/src/bin

	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"
	export PKG_CONFIG_PATH=/opt/linphone-1.3.5/lib/pkgconfig

	SVNrevision=$(svn info "${svn_dir}"/trunk/src |grep ^Revision | cut -d" " -f2)

	MakeRelease="${mkr_dir}/MakeRelease"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p ${svn_dir}/trunk -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" -c ${build_dir}/${flavor}.conf

	LinphoneVersion="$(dpkg -s 'liblinphone1-lmce' | grep '^Version: ' | cut -d' ' -f2)"
	if [[ "$LinphoneVersion" == "1."[57]"."* ]]; then
		# Linphone 1.5.1 in Ubuntu Feisty makes SimplePhone to need this
		export LINPHONE_CONST=const
	fi

	rm -rf $out_dir
	mkdir -p $out_dir
#	336,337 - IVTV Video Drivers
#	589,590 - Alsa Sound Drivers
# 	515,516 - PWC Camera Drivers
#	432  	Pluto VideoLan Client
#	431 	Pluto VideoLan Client Source
#	427 	Pluto VideoLan Plugin
#	426 	Pluto VideoLan Plugin Source
#	430 	Pluto VideoLan Server
#	429 	Pluto VideoLan Server Source
#	462  	Pluto ATI Video Drivers
#	607 	Pluto ATI Video Drivers Source
#	543  	Pluto vloopback Kernel Module
#	542 	Pluto vloopback Kernel Module Source

	$MakeRelease "${MakeReleaseExtraParams[@]}" -R "$SVNrevision" -h $sql_slave_host -u $sql_slave_user -O $out_dir -D $sql_slave_db -o 15 -r 21 -m 1,1108 -K "543,542,462,607,432,431,427,426,430,429,589,590,515,516"  -s "${svn_dir}/trunk" -n / > >(tee -a $build_dir/Build.log)  -d || exit 1
}

function Create_Fake_Windows_Binaries {

	touch ${svn_dir}/trunk/src/bin/Pluto_S60.sis
	touch ${svn_dir}/trunk/src/bin/Orbiter.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_Treo.CAB

	touch ${svn_dir}/trunk/src/bin/UpdateBinary.exe
	touch ${svn_dir}/trunk/src/bin/UpdateBinaryCE.exe 
	touch ${svn_dir}/trunk/src/bin/UpdateBinaryCE_x86.exe
	touch ${svn_dir}/trunk/src/bin/{Orbiter_Win32.dat,Orbiter_CeNet4_XScale.dat}
	touch "${svn_dir}/trunk/src/bin/Symbian Series 60 mobile.vmc"
	touch "${svn_dir}/trunk/src/bin/Windows Mobile Smartphone.vmc"
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.dat

	touch ${svn_dir}/trunk/src/bin/OrbiterInstaller.msi
	touch ${svn_dir}/trunk/src/bin/OrbiterCE_SDL.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_XScale.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.CAB
	touch ${svn_dir}/trunk/src/bin/ImportContacts.zip

	touch ${svn_dir}/trunk/src/bin/Orbiter.exe
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_XScale.exe
	touch ${svn_dir}/trunk/src/bin/OrbiterSmartphone.exe
	touch ${svn_dir}/trunk/src/bin/OrbiterCE_SDL.exe
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.exe

	touch ${svn_dir}/trunk/src/bin/AYGSHELL.DLL
	touch ${svn_dir}/trunk/src/bin/PthreadsCE.dll
	touch ${svn_dir}/trunk/src/bin/Orbiter.MD5
	touch ${svn_dir}/trunk/src/bin/logo.gif

	touch ${svn_dir}/trunk/src/bin/PlutoBaSInstaller.msi
	touch ${svn_dir}/trunk/src/bin/PlutoRebootSetup.msi


	pushd ${svn_dir}/trunk/src/bin
	scp pluto@10.0.2.4:'/home/builds/Windows_Output_LinuxMCE/src/bin/*' ./
	popd

	pushd ${svn_dir}/trunk/src/lib
	scp pluto@10.0.2.4:'/home/builds/Windows_Output_LinuxMCE/src/lib/*' ./
	popd
	
}

function Create_Local_Repository {
	
	rm -f $local_mirror_dir/*.deb
	rm -f $local_mirror_dir/Packages*

	mkdir -p $local_mirror_dir

	cp ${out_dir}/tmp/*.deb $local_mirror_dir
	cp ${replacements_dir}/*.deb $local_mirror_dir
	pushd $local_mirror_dir
		dpkg-scanpackages . /dev/null > Packages
		cat Packages | gzip -9c > Packages.gz
#		ln -sf . $local_mirror_dir/ubuntu
	popd

#       cp -f /root/build-files/virus_free.php /var/www
}

function Copy_Closed_Source_Debs {
	# copy closed source debs
	local ID=$(date +%Y%m%d%H%M%S)
	local priv_debs=(fiire-drivers pluto-wavetrend-reader pluto-zwave-lighting)
	local pkg

	rm -rf "$priv_debs_dir"
	mkdir -p "$priv_debs_dir"
	for pkg in "${priv_debs[@]}"; do
		cp "$out_dir"/tmp/"$pkg"_*.deb "$priv_debs_dir/"
	done

	local Dir="/var/www/priv_debs"
	local Latest="$Dir/latest"

	mkdir -p "$Dir"
	tar -C "$priv_debs_dir" -cvf "$Dir"/"$ID".tar .
	rm -f "$Latest"
	echo "$ID" >"$Latest"
}

function Import_Build_Database {
	local temp_sqlcvsdir=$(mktemp -d)
	local temp_file=$(mktemp)
	local temp_file_main=$(mktemp)
	local temp_file_myth=$(mktemp)
	local temp_file_media=$(mktemp)
	local temp_file_security=$(mktemp)
	local temp_file_telecom=$(mktemp)

	## Import other databases
	mysqldump --compress -h $sql_master_host -u $sql_master_user 'main_sqlcvs' > $temp_file_main
	mysqldump --compress -h $sql_master_host -u $sql_master_user 'myth_sqlcvs' > $temp_file_myth
	mysqldump --compress -h '10.0.2.4' -u 'root' 'pluto_main' > $temp_file        
	mysqldump --compress -h $sql_master_host -u $sql_master_user $sql_master_db_media > $temp_file_media
	mysqldump --compress -h $sql_master_host -u $sql_master_user $sql_master_db_security > $temp_file_security
	mysqldump --compress -h $sql_master_host -u $sql_master_user $sql_master_db_telecom > $temp_file_telecom

	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p $(dirname $temp_file) -e "$(basename $temp_file),$(basename $temp_file_main),$(basename $temp_file_myth),$(basename $temp_file_media),$(basename $temp_file_security),$(basename $temp_file_telecom)" -c ${build_dir}/${flavor}.conf

	echo "DROP DATABASE $sql_slave_db;
		DROP DATABASE $sql_slave_db_mainsqlcvs;
		DROP DATABASE $sql_slave_db_media;
		DROP DATABASE $sql_slave_db_security;
		DROP DATABASE $sql_slave_db_telecom;
		DROP DATABASE $sql_slave_db_mythsqlcvs;
		DROP DATABASE pluto_main;
	" | mysql -f -h $sql_slave_host -u $sql_slave_user
	echo "CREATE DATABASE $sql_slave_db;
		CREATE DATABASE $sql_slave_db_mainsqlcvs;
		CREATE DATABASE $sql_slave_db_mythsqlcvs;
		CREATE DATABASE $sql_slave_db_media;
		CREATE DATABASE $sql_slave_db_security;
		CREATE DATABASE $sql_slave_db_telecom;
		CREATE DATABASE pluto_main;
	" | mysql -f -h $sql_slave_host -u $sql_slave_user
	cat $temp_file | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db
	cat $temp_file | mysql -h $sql_slave_host -u $sql_slave_user "pluto_main"
	cat $temp_file_main | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mainsqlcvs
	cat $temp_file_myth | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mythsqlcvs
	cat $temp_file_media | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_media
	cat $temp_file_security | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_security
	cat $temp_file_telecom | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_telecom

	echo 'DELETE FROM `Package_Version`; DELETE FROM `Schema`;' | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db_mainsqlcvs

	rm -rf $temp_file $temp_file_main $temp_file_myth $temp_file_media $temp_file_security $temp_file_telecom $temp_sqlcvsdir
}

function Import_Pluto_Skins {

	local skins_dir=/home/samba/www_docs/graphics

	mkdir -p /usr/pluto/orbiter/
	rm -f /usr/pluto/orbiter/skins
	ln -s $skins_dir /usr/pluto/orbiter/skins

	rm -rf $skins_dir
	mkdir -p $skins_dir

	pushd /
	ssh pluto@10.0.2.4 tar -c $skins_dir | tar -x
	popd
	
	pushd /usr/pluto/orbiter/skins
#		rm -f Basic
		cp -r LinuxMCE/* Basic/
#		ln -s LinuxMCE Basic
	popd

	pushd ${build_dir}
		ln -s /home/samba
	popd

	mkdir -p "/home/samba/www_docs/sample media"
	pushd /
		ssh pluto@10.0.2.4 'tar -c "/home/samba/www_docs/sample media"' | tar -x 
	popd
	
}

function Create_Diskless_Archive {


	apt-get -y install debootstrap

	local temp_dir=$(mktemp -d)
	debootstrap gutsy $temp_dir http://ro.archive.ubuntu.com/ubuntu/

	rm -f $temp_dir/var/cache/apt/archives/*.deb

	mkdir -p /home/DisklessFS
	pushd $temp_dir
		tar -jcf /home/DisklessFS/PlutoMD_Debootstraped.tar.bz2 *
	popd
	rm -rf $temp_dir
	
	touch /home/DisklessFS/ramdisk.tar.bz2
	touch /home/DisklessFS/PlutoMD.tar.bz2
	mkdir -p  /home/DisklessFS/BootWait
	touch /home/DisklessFS/BootWait/vmlinuz-default-2.6.16.20-pluto-2-686-smp
	touch /home/DisklessFS/BootWait/initrd.img-default-2.6.16.20-pluto-2-686-smp
	
}

function Create_ISO {
	# Create the iso
	pushd $local_mirror_dir
	        /root/Ubuntu_Helpers/cd1-build.sh $iso_name
		/root/Ubuntu_Helpers/cd2-build.sh $iso_name
	popd
}

function Upload_Build_Archive {
	pushd $local_mirror_dir
		tar -zcf /var/plutobuild/linuxmce-uploads.tar.gz *.iso
		scp -i /root/.ssh/uploads_plutohome_key /var/plutobuild/linuxmce-uploads.tar.gz uploads@plutohome.com:
	popd
}

function Backup_Last_Build {
	local PACKAGES_ISO_NAME="${iso_name}-packages.iso"
	local CACHE_ISO_NAME="${iso_name}-cache.iso"
	local BUILD_BACKUP_DIR="/var/www/BuildBackups"


	if [[ ! -f "/var/www/${PACKAGES_ISO_NAME}" ]] ;then
		echo "WARNING: Missing '$PACKAGES_ISO_NAME', skiping backup procedure"
		return
	fi

	local lastPackagesIso_Date=$(stat -c %y /var/www/${PACKAGES_ISO_NAME} | cut -d' ' -f1)
	local lastPackagesIso_Time=$(stat -c %y /var/www/${CACHE_ISO_NAME} | cut -d' ' -f2 | cut -d':' -f1,2)

	mkdir -p "${BUILD_BACKUP_DIR}/${lastPackagesIso_Date}"
	cp "/var/www/${PACKAGES_ISO_NAME}" "${BUILD_BACKUP_DIR}/${lastPackagesIso_Date}/${lastPackagesIso_Date}_${lastPackagesIso_Time}_${PACKAGES_ISO_NAME}"
	cp "/var/www/${CACHE_ISO_NAME}" "${BUILD_BACKUP_DIR}/${lastPackagesIso_Date}/${lastPackagesIso_Date}_${lastPackagesIso_Time}_${CACHE_ISO_NAME}"
	cp "/var/www/kubuntu-linuxmce.iso" "${BUILD_BACKUP_DIR}/${lastPackagesIso_Date}/${lastPackagesIso_Date}_${lastPackagesIso_Time}_kubuntu-linuxmce.iso"
}
