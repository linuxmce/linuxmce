#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/build-scripts/name-packages.sh

set -e
set -x

function build_main_debs() {
	export PATH=$PATH:${svn_dir}/${svn_branch_name}/src/bin
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/${svn_branch_name}/src/lib"

	#FIXME Hackozaurus to build SimplePhone
	export PKG_CONFIG_PATH=/opt/linphone-1.3.5/lib/pkgconfig
	LinphoneVersion="$(dpkg -s 'liblinphone1-lmce' | grep '^Version: ' | cut -d' ' -f2)"
	if [[ "$LinphoneVersion" != "1.3."* ]]; then
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
	"${mkr_dir}/MakeRelease_PrepFiles" -p "${svn_dir}/${svn_branch_name}" \
		-e "*.prep,*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" \
		-c "/etc/lmce-build/${flavor}.conf" || Error "MakeRelease_PrepFiles failed"

	# Clear the debs output directory
	DisplayMessage "Cleaning MakeRelease debs output directory"
	rm -rf "${out_dir}" || Error "Cannot clean MakeRelease debs output directory"
	mkdir -p "${out_dir}" || Error "Cannot create MakeRelease debs output directory"

	# Prepare some params values for MakeRelease
	DisplayMessage "Compiling and building packages"
	SVNrevision=$(svn info "${svn_dir}"/${svn_branch_name}/src |grep ^Revision | cut -d" " -f2)

	exclude_list=$mkr_videolan_plugin_common
	exclude_list=$exclude_list,$mkr_videolan_plugin_server
	exclude_list=$exclude_list,$mkr_videolan_plugin_client
	exclude_list=$exclude_list,$mkr_pwc_camera_drivers
	exclude_list=$exclude_list,$mkr_ati_video_drivers
	exclude_list=$exclude_list,$mkr_alsa_drivers
	exclude_list=$exclude_list,$mkr_diskless_default_boot_file_package
	exclude_list=$exclude_list,$mkr_diskless_base_filesystem_package

	case "$(lsb_release -c -s)" in
		"gutsy")
			Distro_ID="15"
			;;
		"hardy")
			Distro_ID="16"
			;;
		"intrepid")
			Distro_ID="16"
			exclude_list=$exclude_list,$mkr_tira
			exclude_list=$exclude_list,$mkr_shift_key_state
			exclude_list=$exclude_list,$mkr_simplephone
			exclude_list=$exclude_list,$mkr_launch_manager_package
			exclude_list=$exclude_list,$mkr_irtrans_package
			exclude_list=$exclude_list,$mkr_vdr_plugin
			;;
	esac

	# Compile the packages
	"${mkr_dir}/MakeRelease" -a -R "$SVNrevision" -h 'localhost' -u 'root' -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1 -K "$exclude_list" -s "${svn_dir}/${svn_branch_name}" -n / -d || Error "MakeRelease failed"

	# Compile the private packages
	if [[ "$svn_private_url" != "" ]] && [[ "$svn_private_user" != "" ]] && [[ "$svn_private_pass" != "" ]] ;then
		"${mkr_dir}/MakeRelease" -a -R "$SVNrevision" -h 'localhost' -u 'root' -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1108 -K "$exclude_list" -s "${svn_dir}/${svn_branch_name}" -n / -d || Error "MakeRelease failed"
	fi
}


DisplayMessage "*** STEP: Running MakeRelease"
trap 'Error "Undefined error in $0"' EXIT

build_main_debs

trap - EXIT
