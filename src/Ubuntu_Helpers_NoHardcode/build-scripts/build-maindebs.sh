#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/build-scripts/name-packages.sh
. /etc/lmce-build/ubuntu.conf
LC_ALL=C

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

#  export SNR_CPPFLAGS="$compile_defines"

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
#	"${mkr_dir}/MakeRelease_PrepFiles" -p "${svn_dir}/${svn_branch_name}" \
#		-e "*.prep,*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" \
#		-c "/etc/lmce-build/${flavor}.conf" || Error "MakeRelease_PrepFiles failed"
#
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

	# The default version string is 2.0.0.44 and gets amended by the svn revision plus time of day and date
	Main_Version='2.0.0.44.'
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
			# USB Game Pad
			exclude_list=$exclude_list,795,796
#			exclude_list=$exclude_list,$mkr_shift_key_state
#			exclude_list=$exclude_list,$mkr_simplephone
#			exclude_list=$exclude_list,$mkr_launch_manager_package
#			exclude_list=$exclude_list,$mkr_irtrans_package
			;;
		"precise")
			Distro_ID="18"
			Main_Version='2.0.0.45.'
			# mess
			exclude_list=$exclude_list,716,717
			# mame
			exclude_list=$exclude_list,682,683
			# videolan client
			exlucde_list=$exclude_list,431,432
			exclude_list=$exclude_list,$mkr_tira
#			exclude_list=$exclude_list,$mkr_usb_uirt_0038_package
			exclude_list=$exclude_list,$mkr_vloopback_driver_package
#			exclude_list=$exclude_list,672,674 #

			# g++ -c -I.. -I../DCE -I/usr/include/mysql -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES  -Wall -fPIC -ggdb3  PLCBUS.cpp -o PLCBUS.o
			# PLCBUS.cpp: In function ‘void* start(void*)’:
			# PLCBUS.cpp:39:18: error: ‘base’ was not declared in this scope
			# PLCBUS.cpp:39:37: error: expected type-specifier
			# PLCBUS.cpp:39:37: error: expected ‘>’
			# PLCBUS.cpp:39:37: error: expected ‘(’
			# PLCBUS.cpp:39:52: error: expected primary-expression before ‘>’ token
			# PLCBUS.cpp:39:56: error: expected ‘)’ before ‘;’ token
			exclude_list=$exclude_list,767,768
			# g++ -c -I.. -I../DCE -I/usr/include/mysql -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES -I/opt/owfs/include  -Wall -fPIC -ggdb3  OneWire.cpp -o OneWire.o
			# OneWire.cpp: In function ‘void* start(void*)’:
			# OneWire.cpp:30:20: error: ‘base’ was not declared in this scope
			# OneWire.cpp:30:39: error: expected type-specifier
			# OneWire.cpp:30:39: error: expected ‘>’
			# OneWire.cpp:30:39: error: expected ‘(’
			# OneWire.cpp:30:56: error: expected primary-expression before ‘>’ token
			# OneWire.cpp:30:60: error: expected ‘)’ before ‘;’ token
			# OneWire.cpp: In member function ‘virtual void DCE::OneWire::readDevices()’:
			# OneWire.cpp:297:20: error: expected type-specifier
			# OneWire.cpp:297:20: error: expected ‘>’
			# OneWire.cpp:297:20: error: expected ‘(’
			# OneWire.cpp:297:37: error: expected primary-expression before ‘>’ token
			exclude_list=$exclude_list,776,777
			exclude_list=$exclude_list,685,684
			# tribune xml
			exclude_list=$exclude_list,598,599
			# simplephone
			exclude_list=$exclude_list,498,499
			# hal device finder
			exclude_list=$exclude_list,535,536
			# pluto mcr remote
			exclude_list=$exclude_list,653,654
			# orbiter
			exclude_list=$exclude_list,119,138
			# pluto disk drive
			exclude_list=$exclude_list,122
			# pluto bluetooth dongle
			exclude_list=$exclude_list,124
			# orbitergen
			# DesignObj_Generator.cpp:2101:38: error: taking address of temporary [-fpermissive]			
			exclude_list=$exclude_list,226
			# convertmac  -Wno-deprecated
			exclude_list=$exclude_list,315,316
			# sql2cpp missing lib entries
			exclude_list=$exclude_list,202,203
			#/usr/bin/ld: cannot find -lSDL_Helpers
			#collect2: ld returned 1 exit status
			#make: *** [Proxy_Orbiter] Error 1
			#make clean; make bin ***FAILED***
			#Error: make clean; make bin failed!
			#BuildSource failed for: 488 Pluto Proxy Orbiter
			exclude_list=$exclude_list,488
			# /usr/bin/ld: cannot find -lDisk_Drive_Functions
			# BuildSource failed for: 493 Pluto Powerfile C200
			exclude_list=$exclude_list,493
			# usb game pad
#			exclude_list=$exclude_list,795,796
			# 674  # game
#			# does not compile atm
#			exclude_list=$exclude_list,683
			# SDLMESS 
			# does not compile atm
#			exclude_list=$exclude_list,717
                        # MAME SNAPS and metadata
                        exclude_list=$exclude_list,680,681
			# 721 LinuxMCE DPMS Monitor Source
#			exclude_list=$exclude_list,721,722
			;;
	esac


	# Set version of packages to todays date, plus 00:19 as time
	Q="Update Version Set VersionName= concat('$Main_Version',substr(now()+0,3,6),'$SVNrevision') Where PK_Version = 1;"
	mysql $PLUTO_BUILD_CRED -D 'pluto_main_build' -e "$Q"

	# Compile the packages
	echo "\"${mkr_dir}/MakeRelease\" -a -R \"$SVNrevision\" $PLUTO_BUILD_CRED -O \"$out_dir\" -D 'pluto_main_build' -o \"$Distro_ID\" -r 21 -m 1 -K \"$exclude_list\" -s \"${svn_dir}/${svn_branch_name}\" -n / -d"
	arch=$arch "${mkr_dir}/MakeRelease" -a -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1 -K "$exclude_list" -s "${svn_dir}/${svn_branch_name}" -n / -d || Error "MakeRelease failed"

	# Compile the private packages
	if [ "$svn_private_url" -a "$svn_private_user" -a "$svn_private_pass" ]
	then
		echo "\"${mkr_dir}/MakeRelease\" -a -R \"$SVNrevision\" $PLUTO_BUILD_CRED -O \"$out_dir\" -D 'pluto_main_build' -o \"$Distro_ID\" -r 21 -m 1108 -K \"$exclude_list\" -s \"${svn_dir}/${svn_branch_name}\" -n / -d"
		arch=$arch "${mkr_dir}/MakeRelease" -a -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1108 -K "$exclude_list" -s "${svn_dir}/${svn_branch_name}" -n / -d || Error "MakeRelease failed on private packages"
	fi
}


DisplayMessage "*** STEP: Running MakeRelease"
trap 'Error "Undefined error in $0"' EXIT

build_main_debs

trap - EXIT
