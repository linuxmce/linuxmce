#!/bin/bash -e

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/common/utils.sh

set -e

cache_name=".cache"

function Changed_Since_Last_Build
{
	return $(/bin/true) #Zaerc HACK

	local fs_path="$1"
	DisplayMessage "Checking build stamp on '$fs_path'"
	local cache_file="${replacements_dir}/${cache_name}"
	local url_id=$(svn info "$fs_path" | grep '^URL: ' | cut -d' ' -f2 | md5sum | cut -d' ' -f1)
	local revision_new=$(svn info $fs_path | grep '^Revision: ' | cut -d' ' -f2)
	local revision_old=$(cat "$cache_file" | grep "^${url_id}" | cut -d'|' -f2)

	if [ "$revision_new" = "$revision_old" ] ; then
		return $(/bin/false)
	fi

	return $(/bin/true)
}

function Update_Changed_Since_Last_Build
{
#	return $(/bin/true)

        local fs_path="$1"
        DisplayMessage "Updating build stamp for '$fs_path'"
        local cache_file="${replacements_dir}/${cache_name}"
        local url_id=$(svn info "$fs_path" | grep '^URL: ' | cut -d' ' -f2 | md5sum | cut -d' ' -f1)
        local revision_new=$(svn info $fs_path | grep '^Revision: ' | cut -d' ' -f2)

        [[ -f "$cache_file" ]] && grep -v "$url_id" "$cache_file" > "${cache_file}.tmp" || :
        echo "$url_id|$revision_new" >> "${cache_file}.tmp"
        mv "${cache_file}.tmp" "$cache_file"
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
		Update_Changed_Since_Last_Build "$dir_"
	fi
}

function Build_Replacements_Common_ubuntu
{
	mkdir -pv "$replacements_dir"

	#Package: tee-pluto
	Build_Replacement_Package tee-pluto misc_utils/tee-pluto

	#Package: pluto-asterisk
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/asterisk"
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building pluto-asterisk"
		pushd "$dir_"
		./make_package_ubuntu.sh $KVER
		cp -r asterisk-pluto_*.deb ${replacements_dir}
		popd
		Update_Changed_Since_Last_Build "$dir_"
	fi
###	Build_Replacement_Package asterisk-pluto ubuntu/asterisk

	#Package: lirc-pluto
	Build_Replacement_Package lirc-pluto ubuntu/lirc-pluto-0.1

	#Package: mce-launcher
	Build_Replacement_Package mce-launcher src/mce-launcher

	#Package: mce-installer
	Build_Replacement_Package mce-installer src/mce-installer

	#Package: mtx-pluto
	Build_Replacement_Package mtx-pluto ubuntu/mtx-1.3.11

	# TODO Fix this package so it builds against target kernel not running kernel
	#Package: linux-image-diskless
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/linux-image-dummy
	if Changed_Since_Last_Build "$dir_" ;then
		DisplayMessage "Building linux-image-diskless for $KVER"
		pushd "$dir_"
		./makepackage.sh
		cp linux-image-diskless_*.deb "${replacements_dir}"
		popd
		Update_Changed_Since_Last_Build "$dir_"
	fi

	# Yes possy, I am just testing...
	# Build_Replacement_Package pcsxr ubuntu/pcsxr-1.9.93
}

function Build_Replacements_Lucid
{
	mkdir -pv "$replacements_dir"

#	#Package: lirc
	Build_Replacement_Package lirc ubuntu/lirc-0.8.6+lmce1
	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-x*.deb ${replacements_dir}
	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-modules*.deb ${replacements_dir}

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


	if [ "1" == "0" ]; then
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
	#	Build_Replacement_Package vdr-addon-noad ubuntu/vdr-addon-noad-0.7.2
	#	Build_Replacement_Package vdr-plugin-xineliboutput ubuntu/vdr-plugin-xineliboutput-1.0.4+cvs20091215.2049
		Build_Replacement_Package vdr-plugin-xineliboutput ubuntu/vdr-plugin-xineliboutput-1.0.6+cvs20100608.2009
		Build_Replacement_Package vdr-plugin-wirbelscan ubuntu/vdr-plugin-wirbelscan-0.0.5~pre12a
		Build_Replacement_Package vdr-plugin-iptv ubuntu/vdr-plugin-iptv-0.4.2

		cp ${svn_dir}/${svn_branch_name}/ubuntu/libxine*.deb ${replacements_dir}
		cp ${svn_dir}/${svn_branch_name}/ubuntu/xineliboutput-*.deb ${replacements_dir}
	fi
	Build_Replacement_Package vdrnfofs ubuntu/vdrnfofs-0.5

	# SqueezeSlave
	Build_Replacement_Package squeezeslave ubuntu/squeezeslave

	# shairport (AirPlay Audio)
	Build_Replacement_Package shairport ubuntu/shairport-0.05

	# lmce-asterisk
	Build_Replacement_Package lmce-asterisk src/lmce-asterisk
        cp ${svn_dir}/${svn_branch_name}/src/lmce-asterisk*.deb ${replacements_dir}
        Build_Replacement_Package chan-sccp-b ubuntu/asterisk/chan-sccp-b_V4.1
	#Package: zaptel-modules
	DisplayMessage "NOT Building zaptel-modules"
#	m-a --non-inter -ft -l $KVER a-b zaptel
#	cp /usr/src/zaptel-modules*.deb "${replacements_dir}"

	#Package: alsa-modules
	if [[ -f alsa-driver.tar.bz2 ]]; then
		DisplayMessage "Building alsa-modules from existing drivers archive"
		m-a --non-inter -t -l $KVER a-b alsa
		cp /usr/src/alsa-modules*.deb "${replacements_dir}"
	else
		DisplayMessage "Not building alsa-modules, as the drivers archive does not exist"
	fi
	Build_Replacement_Package python-coherence ubuntu/Coherence-0.6.6.2	        

	#Package: lshwd
	Build_Replacement_Package lshwd_2.0 ubuntu/lshwd-2.0-rc4

	#Package: libowfs
	Build_Replacement_Package libowfs27 external/owfs-2.8p5
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/*ow*.deb
	dir_="${svn_dir}/${svn_branch_name}/external"
	cp $dir_/*ow*.deb "${replacements_dir}"
}

function Build_Replacements_Precise
{
	mkdir -pv "$replacements_dir"

#	#Package: lirc
	Build_Replacement_Package lirc ubuntu/lirc-0.9.0-0ubuntu1+lmce1
	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-x*.deb ${replacements_dir}
	# We have no lirc-modules for 0.9
#	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-modules*.deb ${replacements_dir}

        apt-get install -y liblinphone-dev

	Build_Replacement_Package vdrnfofs ubuntu/vdrnfofs-0.5

	# SqueezeSlave
	Build_Replacement_Package squeezeslave ubuntu/squeezeslave-1.3-393

	# shairport (AirPlay Audio)
# not building as libao2 is no longer with us. Maybe libao4 works?!
#	Build_Replacement_Package shairport ubuntu/shairport-0.05

	# lmce-asterisk
	Build_Replacement_Package lmce-asterisk src/lmce-asterisk
	cp ${svn_dir}/${svn_branch_name}/src/lmce-asterisk*.deb ${replacements_dir}


	# qOrbiter doesn't have compatibility record yet.
	#Build_Replacement_Package qorbiter src/qOrbiter
	#cp ${svn_dir}/${svn_branch_name}/src/qorbiter*.deb ${replacements_dir}

	#Package: zaptel-modules

        Build_Replacement_Package chan-sccp-b ubuntu/asterisk/chan-sccp-b_V4.1

	Build_Replacement_Package python-coherence ubuntu/Coherence-0.6.6.2

	# Open ZWave library
	Build_Replacement_Package libopenzwave1.0 external/open-zwave
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/libopenzwave1.0*.deb

	Build_Replacement_Package libhupnp-core external/hupnp/hupnp
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/hupnp/libhupnp-core*.deb

	Build_Replacement_Package libhupnp-av external/hupnp/hupnp_av
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/hupnp/libhupnp-av*.deb

	Build_Replacement_Package ola external/ola-0.9.0
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/ola-dev*.deb
	dir_="${svn_dir}/${svn_branch_name}/external/ola"
	cp $dir_/ola*.deb "${replacements_dir}"

	#Package: libbluray1
	Build_Replacement_Package libbluray1 ubuntu/libbluray-0.5.0
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/*bluray*.deb
	dir_="${svn_dir}/${svn_branch_name}/ubuntu"
	cp $dir_/*bluray*.deb "${replacements_dir}"

	# precise libsmbclient doesn't ship a pkg-config file, but xine checks for it, so lets provide one
	cp ${svn_dir}/${svn_branch_name}/ubuntu/smbclient.pc /usr/lib/pkgconfig/
	#Package: libxine2
	Build_Replacement_Package libxine2 ubuntu/xine-lib-1.2.4
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/*xine*.deb
	dir_="${svn_dir}/${svn_branch_name}/ubuntu"
	cp $dir_/*xine*.deb "${replacements_dir}"

	#Obsolete
	#Package: lshwd
	#Build_Replacement_Package lshwd_2.0 ubuntu/lshwd-2.0-rc4

	#owfs 2.8p5 is no longer needed as it is in the ubuntu precise/universe repo
	##Package: libowfs
	#Build_Replacement_Package libowfs27 external/owfs-2.8p5
	#dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/*ow*.deb
	#dir_="${svn_dir}/${svn_branch_name}/external"
	#cp $dir_/*ow*.deb "${replacements_dir}"
}

function Build_Replacements_Intrepid
{
	mkdir -pv "$replacements_dir"


	#Package: djmount
	Build_Replacement_Package djmount ubuntu/djmount-0.71
	Build_Replacement_Package chan-sccp-b ubuntu/asterisk/Chan_SCCP-3.0.4_STABLE


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
		Update_Changed_Since_Last_Build "$dir_"
	fi

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

	#Package: lshwd
	Build_Replacement_Package lshwd_2.0 ubuntu/lshwd-2.0-rc4
}

function Build_Replacements_trusty
{
	mkdir -pv "$replacements_dir"

#	#Package: lirc
	Build_Replacement_Package lirc ubuntu/lirc-0.9.0-0ubuntu1+lmce1
	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-x*.deb ${replacements_dir}
	# We have no lirc-modules for 0.9
#	cp ${svn_dir}/${svn_branch_name}/ubuntu/lirc-modules*.deb ${replacements_dir}

        apt-get install -y liblinphone-dev

	Build_Replacement_Package vdrnfofs ubuntu/vdrnfofs-0.5

	# SqueezeSlave
	Build_Replacement_Package squeezeslave ubuntu/squeezeslave-1.3-393

	# shairport (AirPlay Audio)
	Build_Replacement_Package shairport ubuntu/shairport-0.05

	# lmce-asterisk
	Build_Replacement_Package lmce-asterisk src/lmce-asterisk
        cp ${svn_dir}/${svn_branch_name}/src/lmce-asterisk*.deb ${replacements_dir}

	# qOrbiter doesn't have compatibility record yet.
	# Build_Replacement_Package qorbiter src/qOrbiter
        # cp ${svn_dir}/${svn_branch_name}/src/qorbiter*.deb ${replacements_dir}

        Build_Replacement_Package chan-sccp-b ubuntu/asterisk/chan-sccp-b_V4.1

	#Build_Replacement_Package python-coherence ubuntu/Coherence-0.6.6.2

	# Open ZWave library
	Build_Replacement_Package libopenzwave1.0 external/open-zwave
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/libopenzwave1.0*.deb

	# libhupnp and libhupnp-av need to build under qt4.
	QT_SELECT=4 Build_Replacement_Package libhupnp-core external/hupnp/hupnp
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/hupnp/libhupnp-core*.deb

	QT_SELECT=4 Build_Replacement_Package libhupnp-av external/hupnp/hupnp_av
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/hupnp/libhupnp-av*.deb

	Build_Replacement_Package ola external/ola-0.9.0
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/ola-dev*.deb
	dir_="${svn_dir}/${svn_branch_name}/external/ola"
	cp $dir_/ola*.deb "${replacements_dir}"

	# Package: libhal1, libhal-dev....  ugh.  Need a new hw detection daemon!
	Build_Replacement_Package hal ubuntu/hal-0.5.14
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/*hal*.deb
	dir_="${svn_dir}/${svn_branch_name}/ubuntu"
	cp $dir_/*hal*.deb "${replacements_dir}"

	#Package: lshwd
	#Build_Replacement_Package lshwd_2.0 ubuntu/lshwd-2.0-rc4

	#owfs 2.8p5 is no longer needed as it is in the ubuntu trusty/universe repo
	##Package: libowfs
	#Build_Replacement_Package libowfs27 external/owfs-2.8p5
	#dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/*ow*.deb
	#dir_="${svn_dir}/${svn_branch_name}/external"
	#cp $dir_/*ow*.deb "${replacements_dir}"

}

trap 'Error "Undefined error in $0" ; apt-get install libtool -y' EXIT

DisplayMessage "*** STEP: Building replacement debs"
# make sure the headers are there
apt-get --force-yes -y install linux-headers-$KVER 

# dir is required even if no replacements are built
mkdir -pv "${replacements_dir}"

case "${flavor}" in
        "ubuntu")
		DisplayMessage "Building Replacements for $build_name"
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
                        "lucid")
                                Build_Replacements_Lucid
                                ;;
                        "precise")
                                Build_Replacements_Precise
                                ;;
                        "trusty")
                                Build_Replacements_trusty
                                ;;
                esac
		DisplayMessage "Building Replacements for $flavor"
                Build_Replacements_Common_ubuntu
                ;;
        "raspbian")
                DisplayMessage "No replacements for raspbian at this time."
                ;;
esac

DisplayMessage "Removing duplicate debs from replacements"
remove_duplicate_debs "${replacements_dir}"

trap - EXIT

