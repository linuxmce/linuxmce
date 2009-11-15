#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/build-scripts/name-packages.sh

set -e
#set -x

PLUTO_BUILD_CRED=""
if [ "$sql_build_host" ] ; then PLUTO_BUILD_CRED="$PLUTO_BUILD_CRED -h $sql_build_host"; fi
if [ "$sql_build_port" ] ; then PLUTO_BUILD_CRED="$PLUTO_BUILD_CRED -P $sql_build_port"; fi
if [ "$sql_build_user" ] ; then PLUTO_BUILD_CRED="$PLUTO_BUILD_CRED -u $sql_build_user"; fi
if [ "$sql_build_pass" ] ; then PLUTO_BUILD_CRED="$PLUTO_BUILD_CRED -p $sql_build_pass"; fi
export PLUTO_BUILD_CRED

MYSQL_BUILD_CRED=""
if [ "$sql_build_host" ] ; then MYSQL_BUILD_CRED="$MYSQL_BUILD_CRED -h$sql_build_host"; fi
if [ "$sql_build_port" ] ; then MYSQL_BUILD_CRED="$MYSQL_BUILD_CRED -P$sql_build_port"; fi
if [ "$sql_build_user" ] ; then MYSQL_BUILD_CRED="$MYSQL_BUILD_CRED -u$sql_build_user"; fi
if [ "$sql_build_pass" ] ; then MYSQL_BUILD_CRED="$MYSQL_BUILD_CRED -p$sql_build_pass"; fi
export MYSQL_BUILD_CRED


function build_main_debs() {
	export PATH=$PATH:${svn_dir}/${svn_branch_name}/src/bin
	echo "PATH=$PATH"
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/${svn_branch_name}/src/lib"
	echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"

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
	echo "\"${mkr_dir}/MakeRelease_PrepFiles\" -p \"${svn_dir}/${svn_branch_name}\" -e \"*.prep,*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk\" -c \"/etc/lmce-build/${flavor}.conf\""
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

	case "${build_name}" in
		"gutsy")
			Distro_ID="15"
			;;
		"hardy")
			Distro_ID="16"
			;;
		"intrepid")
			Distro_ID="17"
			exclude_list=$exclude_list,$mkr_tira
#			exclude_list=$exclude_list,$mkr_shift_key_state
#			exclude_list=$exclude_list,$mkr_simplephone
#			exclude_list=$exclude_list,$mkr_launch_manager_package
			exclude_list=$exclude_list,$mkr_irtrans_package
			;;
	esac

	# Set version of packages to todays date, plus 00:19 as time
	Q="Update Version Set VersionName= concat('2.0.0.44.',substr(now()+0,3,6),'$SVNrevision') Where PK_Version = 1;"
	mysql $PLUTO_BUILD_CRED -D 'pluto_main_build' -e "$Q"

	# Compile the packages
	echo "\"${mkr_dir}/MakeRelease\" -a -R \"$SVNrevision\" $PLUTO_BUILD_CRED -O \"$out_dir\" -D 'pluto_main_build' -o \"$Distro_ID\" -r 21 -m 1 -K \"$exclude_list\" -s \"${svn_dir}/${svn_branch_name}\" -n / -d"
	"${mkr_dir}/MakeRelease" -a -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1 -K "$exclude_list" -s "${svn_dir}/${svn_branch_name}" -n / -d || Error "MakeRelease failed"

	# Compile the private packages
	if [ "$svn_private_url" -a "$svn_private_user" -a "$svn_private_pass" ]
	then
		echo "\"${mkr_dir}/MakeRelease\" -a -R \"$SVNrevision\" $PLUTO_BUILD_CRED -O \"$out_dir\" -D 'pluto_main_build' -o \"$Distro_ID\" -r 21 -m 1108 -K \"$exclude_list\" -s \"${svn_dir}/${svn_branch_name}\" -n / -d"
		"${mkr_dir}/MakeRelease" -a -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1108 -K "$exclude_list" -s "${svn_dir}/${svn_branch_name}" -n / -d || Error "MakeRelease failed on private packages"
	fi
}


DisplayMessage "*** STEP: Running MakeRelease"
trap 'Error "Undefined error in $0"' EXIT

build_main_debs

trap - EXIT
