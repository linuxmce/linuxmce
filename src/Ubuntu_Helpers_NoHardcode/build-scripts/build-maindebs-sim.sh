#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
set -x

function build_main_debs_sim() {
	export PATH=$PATH:${svn_dir}/trunk/src/bin
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"

	#FIXME Hackozaurus to build SimplePhone
	export PKG_CONFIG_PATH=/opt/linphone-1.3.5/lib/pkgconfig
	LinphoneVersion="$(dpkg -s 'liblinphone1-lmce' | grep '^Version: ' | cut -d' ' -f2)"
	if [[ "$LinphoneVersion" == "1."[57]"."* ]]; then
		# Linphone 1.5.1 in Ubuntu Feisty makes SimplePhone to need this
		export LINPHONE_CONST=const
	fi

	#FIXME Hackozaurus for pluto-dummy-console-tools
	mkdir -p /usr/pluto/dummy-packages/
	touch /usr/pluto/dummy-packages/Readme.PlutoConsoleUtilities.dummy

	#FIXME Hackozaurus for pluto-devel-dependencies
	touch /home/README.Devel.Dependencies

	#FIXME Hackozaurus for ubuntu-diskless-tools
	mkdir -p /home/DisklessFS/
	cp "${diskless_dir}/PlutoMD_Debootstraped.tar.bz2" /home/DisklessFS

	# Perform Search&Replace on the sources
	DisplayMessage "Performing search'n'replace on the sources"
	"${mkr_dir}/MakeRelease_PrepFiles" -p "${svn_dir}/trunk" -e "*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" -c "/etc/lmce-build/${flavor}.conf" || Error "MakeRelease_PrepFiles failed"

	# Clear the debs output directory
	DisplayMessage "Cleaning MakeRelease debs output directory"
	rm -rf "${out_dir}" || Error "Cannot clean MakeRelease debs output directory"
	mkdir -p "${out_dir}" || Error "Cannot create MakeRelease debs output directory"

	# Prepare some params values for MakeRelease
	DisplayMessage "Compiling and building packages"
	SVNrevision=$(svn info "${svn_dir}"/trunk/src |grep ^Revision | cut -d" " -f2)
	ExcludePkgList="462,607,432,431,427,426,430,429,589,590,515,516,266,540"

	# Compile the packages
	"${mkr_dir}/MakeRelease" -a -R "$SVNrevision" -h 'localhost' -u 'root' -O "$out_dir" -D 'pluto_main_build' -o 15 -r 21 -m 1 -K "$ExcludePkgList" -s "${svn_dir}/trunk" -n / -d -c || Error "MakeRelease failed: compiling public"

	# Compile the private packages
	if [[ "$svn_private_url" != "" ]] && [[ "$svn_private_user" != "" ]] && [[ "$svn_private_pass" != "" ]] ;then
		"${mkr_dir}/MakeRelease" -a -R "$SVNrevision" -h 'localhost' -u 'root' -O "$out_dir" -D 'pluto_main_build' -o 15 -r 21 -m 1108 -K "$ExcludePkgList" -s "${svn_dir}/trunk" -n / -d -c || Error "MakeRelease failed: compiling private"
	fi

	# Simulate the package creation
	"${mkr_dir}/MakeRelease" -a -R "$SVNrevision" -h 'localhost' -u 'root' -O "$out_dir" -D 'pluto_main_build' -o 15 -r 21 -m 1 -K "$ExcludePkgList" -s "${svn_dir}/trunk" -n / -d -b -S || Error "MakeRelease failed: packaging public"

	if [[ "$svn_private_url" != "" ]] && [[ "$svn_private_user" != "" ]] && [[ "$svn_private_pass" != "" ]] ;then
		"${mkr_dir}/MakeRelease" -a -R "$SVNrevision" -h 'localhost' -u 'root' -O "$out_dir" -D 'pluto_main_build' -o 15 -r 21 -m 1108 -K "$ExcludePkgList" -s "${svn_dir}/trunk" -n / -d -b -S || Error "MakeRelease failed: packaging private"
	fi
}


DisplayMessage "*** STEP: Running MakeRelease"
trap 'Error "Undefined error in $0"' EXIT

build_main_debs_sim

trap - EXIT
