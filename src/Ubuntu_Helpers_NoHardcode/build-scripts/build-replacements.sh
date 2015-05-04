#!/bin/bash -e

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/common/utils.sh

set -e
set -x

make_jobs=""
# set NUMCORES=X in /etc/lmce-build/builder.conf to enable multi-job builds
[[ -n "$NUM_CORES" ]] && [[ "$NUM_CORES" -gt 1 ]] && make_jobs="-j$NUM_CORES"

cache_name=".cache"

function Changed_Since_Last_Build
{
#	return $(/bin/true) #Zaerc HACK

	local fs_path="$1"
	DisplayMessage "Checking build stamp on '$fs_path'"
	local cache_file="${replacements_dir}/${cache_name}"
	local url_id=$(svn info "$fs_path" | grep '^URL: ' | cut -d' ' -f2 | md5sum | cut -d' ' -f1)
	local revision_new=$(svn info $fs_path | grep '^Revision: ' | cut -d' ' -f2)
	local revision_old=$(cat "$cache_file" | grep "^${url_id}" | cut -d'|' -f2)

	if [ "$revision_new" = "$revision_old" ] && [ -n "$revision_old" ]; then
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

	return $(/bin/true)
}

function Build_Replacement_Package
{
	local pkg_name="$1"
	local pkg_dir="$2"


	dir_="${svn_dir}/${svn_branch_name}/${pkg_dir}"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		DisplayMessage "Building ${pkg_name}"
		echo "dpkg-buildpackage -rfakeroot -us -uc -b -tc"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc $make_jobs
		cp -fr ../*${pkg_name}*.deb "${replacements_dir}"
		cp -fr ../*${pkg_name}*.changes "${replacements_dir}"
		Update_Changed_Since_Last_Build "$dir_"
		popd

		return $(/bin/true)
	else
		return $(/bin/false)
	fi
}

function Build_Replacements_Common_ubuntu
{
	mkdir -pv "$replacements_dir"

	#Package: lirc
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/lirc-0.9.0-0ubuntu1+lmce1
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		DisplayMessage "Building lirc-0.9.0-0ubuntu1+lmce1"
		echo "dpkg-buildpackage -rfakeroot -uc -b"
		dpkg-buildpackage -uc -b && \
		cp -fr ${svn_dir}/${svn_branch_name}/ubuntu/*lirc*.deb ${replacements_dir} && \
		cp -fr ${svn_dir}/${svn_branch_name}/ubuntu/*lirc*.changes ${replacements_dir} || :
		Update_Changed_Since_Last_Build "$dir_"
		popd
	fi

	Build_Replacement_Package vdrnfofs ubuntu/vdrnfofs-0.5 || :

	# SqueezeSlave
	Build_Replacement_Package squeezeslave ubuntu/squeezeslave-1.3-393 || :

	# ola needs to be configured to the current build environment
	dir_=${svn_dir}/${svn_branch_name}/external/ola-0.9.0
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		DisplayMessage "Building ola-0.9.0"
		autoreconf -i && \
		dpkg-buildpackage -uc -b -tc $make_jobs && \
		cp -fr ${svn_dir}/${svn_branch_name}/external/ola*.deb "${replacements_dir}" && \
		cp -fr ${svn_dir}/${svn_branch_name}/external/ola*.changes "${replacements_dir}" && \
		Update_Changed_Since_Last_Build "$dir_" || :
		popd
	fi

	#Package: tee-pluto
	Build_Replacement_Package tee-pluto misc_utils/tee-pluto || :

	#Package: pluto-asterisk
	dir_="${svn_dir}/${svn_branch_name}/ubuntu/asterisk"
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		DisplayMessage "Building pluto-asterisk"
		./make_package_ubuntu.sh $KVER && \
		cp -fr asterisk-pluto_*.deb ${replacements_dir} && \
		cp -fr asterisk-pluto_*.changes ${replacements_dir} && \
		Update_Changed_Since_Last_Build "$dir_" || :
		popd
	fi

	#Package: lirc-pluto
	Build_Replacement_Package lirc-pluto ubuntu/lirc-pluto-0.1 || :

	#Package: mtx-pluto
	Build_Replacement_Package mtx-pluto ubuntu/mtx-1.3.11 || :

	# lmce-asterisk
	Build_Replacement_Package lmce-asterisk src/lmce-asterisk || :

	#Package: linux-image-diskless
	dir_=${svn_dir}/${svn_branch_name}/ubuntu/linux-image-dummy
	if Changed_Since_Last_Build "$dir_" ;then
		pushd "$dir_"
		DisplayMessage "Building linux-image-diskless for $KVER"
		./makepackage.sh && \
		cp -fr linux-image-diskless_*.deb "${replacements_dir}" && \
		cp -fr linux-image-diskless_*.changes "${replacements_dir}" && \
		Update_Changed_Since_Last_Build "$dir_" || :
		popd
	fi

	Build_Replacement_Package lmce-mame-metadata extra/mamedata || :

	Build_Replacement_Package lmce-mame-snaps extra/snap || :

	Build_Replacement_Package lmce-sample-media extra/sample_media || :

	Build_Replacement_Package lmce-avwizard-sounds extra/avwizard-sounds || :

	Build_Replacement_Package video-wizard-videos extra/video-wizard-videos || :
}

function Build_Replacements_ubuntu_precise
{
	mkdir -pv "$replacements_dir"

	# lmce-asterisk
	Build_Replacement_Package lmce-asterisk src/lmce-asterisk || :

        Build_Replacement_Package chan-sccp-b ubuntu/asterisk/chan-sccp-b_V4.1 || :

	Build_Replacement_Package python-coherence ubuntu/Coherence-0.6.6.2 || :

	# Open ZWave library
	Build_Replacement_Package libopenzwave1.0 external/openzwave-1.3.1025 && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/libopenzwave1.0*.deb || :

	#Package: libbluray1
	dir_="${svn_dir}/${svn_branch_name}/ubuntu"
	Build_Replacement_Package bluray ubuntu/libbluray-0.5.0 && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/*bluray*.deb || :

	# precise libsmbclient doesn't ship a pkg-config file, but xine checks for it, so lets provide one
	cp ${svn_dir}/${svn_branch_name}/ubuntu/smbclient.pc /usr/lib/pkgconfig/
	#Package: libxine2
	dir_="${svn_dir}/${svn_branch_name}/ubuntu"
	Build_Replacement_Package xine ubuntu/xine-lib-1.2.6 && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/*xine*.deb || :

	#Package: protobuf
	#Build_Replacement_Package protobuf ubuntu/ola/protobuf-2.3.0
	#dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/ola/*proto*.deb
	#dir_="${svn_dir}/${svn_branch_name}/ubuntu/ola"
	#cp $dir_/*proto*.deb "${replacements_dir}"

	#Package: ola
	#Build_Replacement_Package ola ubuntu/ola/ola-0.8.9
	#dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/ola/*ola*.deb
	#dir_="${svn_dir}/${svn_branch_name}/ubuntu/ola"
	#cp $dir_/*ola*.deb "${replacements_dir}"

	#libsoxr-0.1.1/
	#Package: libsoxr-0.1.1 - for squeezelite
	dir_="${svn_dir}/${svn_branch_name}/ubuntu"
	Build_Replacement_Package soxr ubuntu/libsoxr-0.1.1 && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/*soxr*.deb || :

	#ubuntu/squeezelite-1.4
	#Package: squeezelite-1.4
	Build_Replacement_Package squeezelite ubuntu/squeezelite-1.4 && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/squeezelite_1.4*.deb && \
	dir_="${svn_dir}/${svn_branch_name}/ubuntu" || :

	#Package: libcec
	Build_Replacement_Package cec ubuntu/libcec-2.1.4 && \
	dpkg -i ${svn_dir}/${svn_branch_name}/ubuntu/libcec*.deb || :
}

function Build_Replacements_ubuntu_trusty
{
	mkdir -pv "$replacements_dir"

	# shairport (AirPlay Audio)
	Build_Replacement_Package shairport ubuntu/shairport-0.05 || :

        Build_Replacement_Package chan-sccp-b ubuntu/asterisk/chan-sccp-b_V4.1 || :

	#Build_Replacement_Package python-coherence ubuntu/Coherence-0.6.6.2

	# Open ZWave library
	Build_Replacement_Package zwave external/openzwave-1.3.1025 && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/libopenzwave1.0*.deb || :

	# libhupnp and libhupnp-av need to build under qt4.
	QT_SELECT=4 Build_Replacement_Package libhupnp-core external/hupnp/hupnp && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/hupnp/libhupnp-core*.deb || :

	QT_SELECT=4 Build_Replacement_Package libhupnp-av external/hupnp/hupnp_av && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/external/hupnp/libhupnp-av*.deb || :

	# Package: libruby1.8, ruby1.8-dev....  ugh.  GSD breaks on 1.9.1 && 2.0
	dir_="${svn_dir}/${svn_branch_name}/ubuntu"
	Build_Replacement_Package 1.8_1.8.7.375 ubuntu/ruby1.8-1.8.7.375/ && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/*1.8_1.8.7.375*.deb || :

	#Package: libxine2
	dir_="${svn_dir}/${svn_branch_name}/ubuntu"
	Build_Replacement_Package xine ubuntu/xine-lib-1.2.6 && \
	dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/*xine*.deb || :
}

function Build_Replacements_Common_raspbian
{
	: # no-op
}

function Build_Replacements_raspbian_wheezy
{
	mkdir -pv "$replacements_dir"

	#Package: libcec
	Build_Replacement_Package libcec raspbian/libcec-2.1.4 && \
	dpkg -i ${svn_dir}/${svn_branch_name}/raspbian/libcec*.deb && \
	cp ${svn_dir}/${svn_branch_name}/raspbian/*cec*.deb ${replacements_dir} && \
	cp ${svn_dir}/${svn_branch_name}/raspbian/*cec*.changes ${replacements_dir} || :
}

trap 'Error "Undefined error in $0" ; apt-get install libtool -y' EXIT

DisplayMessage "*** STEP: Building replacement debs"
# make sure the headers are there
#apt-get --force-yes -y install linux-headers-$KVER 

# dir is required even if no replacements are built
mkdir -pv "${replacements_dir}"

DisplayMessage "Building Replacements for $flavor $build_name"
Build_Replacements_${flavor}_${build_name}
DisplayMessage "Building Replacements for $flavor"
Build_Replacements_Common_${flavor}

DisplayMessage "Removing duplicate debs from replacements"
remove_duplicate_debs "${replacements_dir}"

trap - EXIT

