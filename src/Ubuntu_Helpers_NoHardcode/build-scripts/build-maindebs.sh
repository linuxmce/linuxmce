#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/build-scripts/name-packages.sh
. /usr/local/lmce-build/build-scripts/version-helper.sh

LC_ALL=C

set -e
#set -x

make_jobs=""
# set NUMCORES=X in /etc/lmce-build/builder.custom.conf to enable multi-job builds
[[ -n "$NUM_CORES" ]] && [[ "$NUM_CORES" -gt 1 ]] && make_jobs="-j $NUM_CORES"

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

export SNR_CPPFLAGS="$compile_defines"

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

#	#FIXME Hackozaurus for ubuntu-diskless-tools
#	mkdir -p /home/DisklessFS/
#	cp "${diskless_dir}/PlutoMD_Debootstraped.tar.bz2" /home/DisklessFS

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
	case "${flavor}" in
		"ubuntu")
                        #FIXME Hackozaurus for ubuntu-diskless-tools
                        mkdir -p /home/DisklessFS/
                        diskless_image_name="PlutoMD_Debootstraped.tar.bz2"
                        cp "${diskless_dir}/$diskless_image_name" /home/DisklessFS
#                       cp "${diskless_dir}/PlutoMD_Debootstraped.tar.bz2" /home/DisklessFS

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
					;;
                                "lucid")
                                        Distro_ID="18"
                                        RepositorySource=21
                                        Main_Version='2.0.0.45.'
					;;
 				"precise")
					Distro_ID="20"
					RepositorySource=25
					Main_Version='2.0.0.46.'
					;;
 				"trusty")
					Distro_ID="20"
					RepositorySource=25
					Main_Version='2.0.0.46.'
					# mame - excluded due to long time for compilation.
					exclude_list=$exclude_list,682,683

					# Not building for some reason or other
					exclude_list=$exclude_list,498,499 # Simplephone - API change
					exclude_list=$exclude_list,782,785 # ola
					;;
			esac
			;;
		"raspbian")
			#FIXME Hackozaurus for ubuntu-diskless-tools
			mkdir -p /home/DisklessFS/
		        diskless_image_name="PlutoMD_Debootstraped-$flavor-$build_name-$arch.tar.bz2"
			cp "${diskless_dir}/$diskless_image_name" /home/DisklessFS

			case "${build_name}" in
				wheezy)
					Distro_ID="19"
					Repository_ID="23"
					Main_Version='2.0.0.46.'

					# not currently compatible
					exclude_list=$exclude_list,819,820,821,822 # AirPlay Streamer/Plugin

					# does not compile and/or is not compatible
		                        exclude_list=$exclude_list,122,142	# Pluto Disk Drive
		                        exclude_list=$exclude_list,728	# pluto hvr-1600 - incompatible - no pci
					exclude_list=$exclude_list,676	# Game Library lmce-game-db - source incompatible??
					exclude_list=$exclude_list,674	# Game Player
		                        exclude_list=$exclude_list,683	# MAME
		                        exclude_list=$exclude_list,133	# MythTV Player
		                        exclude_list=$exclude_list,726	# MythTV Scripts
		                        exclude_list=$exclude_list,685	# EnOceon TCM120 - code error
		                        exclude_list=$exclude_list,751	# HAI Control Program - needs Makefile?
		                        exclude_list=$exclude_list,768	# PLCBUS - code error
		                        exclude_list=$exclude_list,773	# EIB - missing header?
		                        exclude_list=$exclude_list,785	# OLA - missing dep
		                        exclude_list=$exclude_list,794	# LMCE AirPlay Audio Player - missing dep
					exclude_list=$exclude_list,689	# LMCE Datalogger library - says missing libdataloger...
					exclude_list=$exclude_list,696	# LMCE Screen Capture Camera
					exclude_list=$exclude_list,710	# IRTrans Ethernet
					exclude_list=$exclude_list,712	# Chromoflex LED
					exclude_list=$exclude_list,714	# VistaICM2
					exclude_list=$exclude_list,749,748	# hai omni rs232
					exclude_list=$exclude_list,759,758	# pandora
					exclude_list=$exclude_list,796,795	# usb game pad

					# needs pre-compiled binary
					exclude_list=$exclude_list,$mkr_tira
		                        exclude_list=$exclude_list,453	# IRTrans Wrapper

					# not required for an MD
					# need build database fixes to build database packages - not required for an MD
		                        exclude_list=$exclude_list,211	# Pluto System database
		                        exclude_list=$exclude_list,214	# Pluto Local database
		                        exclude_list=$exclude_list,233	# Pluto media database
		                        exclude_list=$exclude_list,256	# Pluto security database
		                        exclude_list=$exclude_list,436	# Pluto Marbella database
		                        exclude_list=$exclude_list,454	# Pluto Audi database
		                        exclude_list=$exclude_list,472	# Pluto telecom database
		                        exclude_list=$exclude_list,679	# Pluto game database
	
					# not building and not required for MD
					exclude_list=$exclude_list,555,556	# AVWizard
					exclude_list=$exclude_list,690	# Generic PC Workstation
		                        exclude_list=$exclude_list,628	# MCE diskless tools - need to update for rpi debootstrapped image
					exclude_list=$exclude_list,704	# Shoutcast Radio Plugin
					exclude_list=$exclude_list,687	# LMCE Datalogger plugin - says 'source not compatible with this distro'
		                        exclude_list=$exclude_list,134	# MythTV Plugin
		                        exclude_list=$exclude_list,680	# MAME SNAPS
		                        exclude_list=$exclude_list,681	# MAME metadata
					exclude_list=$exclude_list,679	# Game Database
					exclude_list=$exclude_list,780	# LinuxMCE mediatagging
					exclude_list=$exclude_list,672	# Game Plugin
					exclude_list=$exclude_list,792,791	# airplay plugin
					exclude_list=$exclude_list,743,742	# hulu plugin
					exclude_list=$exclude_list,722	# LinuxMCE DPMS Monitor

					# building but not required for MD
		                        exclude_list=$exclude_list,204,718,719,727	# Pluto Skins
					exclude_list=$exclude_list,804	# LMCE Skins BlueCrystal
		                        exclude_list=$exclude_list,422  # Pluto Sample Media
		                        exclude_list=$exclude_list,701	# lmce mediatomb
		                        exclude_list=$exclude_list,538,539	# Pluto NewMDInteractor
		                        exclude_list=$exclude_list,753	# datalog database
		                        exclude_list=$exclude_list,490	# Pluto nvidia video drivers
		                        exclude_list=$exclude_list,364	# windows ce libs
		                        exclude_list=$exclude_list,576	# Pluto dynamic dns
		                        exclude_list=$exclude_list,578	# Pluto firewire2v4l
		                        exclude_list=$exclude_list,294	# Pluto qos
		                        exclude_list=$exclude_list,769	# lmce-hybrid
		                        exclude_list=$exclude_list,302	# pluto-dhcpd plugin
		                        exclude_list=$exclude_list,455,456	# pluto motion wrapper
		                        exclude_list=$exclude_list,424,425	# pluto slimserver plugin
		                        exclude_list=$exclude_list,618,619	# msi button
		                        exclude_list=$exclude_list,396,397	# dcegen
		                        exclude_list=$exclude_list,137,118	# xine plugin
		                        exclude_list=$exclude_list,161,162	# pluto gc100
		                        exclude_list=$exclude_list,505,506	# pluto zwave lighting
		                        exclude_list=$exclude_list,270,271	# pluto asterisk
		                        exclude_list=$exclude_list,229,339	# pluto voicemail monitor
		                        exclude_list=$exclude_list,620,621	# pluto hdhomerun
		                        exclude_list=$exclude_list,199,200	# pluto vip shared lib
		                        exclude_list=$exclude_list,143,124	# pluto bluetooth dongle, needs vip shared
		                        exclude_list=$exclude_list,342,341	# pluto updateentarea
		                        exclude_list=$exclude_list,405,406	# pluto updatemedia
		                        exclude_list=$exclude_list,492,493	# pluto powerfilec200
		                        exclude_list=$exclude_list,226,227	# pluto orbitergen
		                        exclude_list=$exclude_list,392,393	# pluto remote assistance
		                        exclude_list=$exclude_list,289,290	# pluto slim server streamer
		                        exclude_list=$exclude_list,487,488	# pluto proxy orbiter
		                        exclude_list=$exclude_list,330,491	# pluto plutodhcp
		                        exclude_list=$exclude_list,154,155	# pluto dcerouter
		                        exclude_list=$exclude_list,206	# pluto development dependencies
		                        exclude_list=$exclude_list,702	# pluto mediatomb
		                        exclude_list=$exclude_list,220	# pluto website admin
		                        exclude_list=$exclude_list,765	# linuxmce internet gateway device support
		                        exclude_list=$exclude_list,116,135	# pluto standard plugins
		                        exclude_list=$exclude_list,639,640	# pluto xml data plugin
		                        exclude_list=$exclude_list,446,447	# pluto makerelease
		                        exclude_list=$exclude_list,202,203	# pluto sql2cpp (needs pluto-ra)
		                        exclude_list=$exclude_list,247,248	# pluto ra
		                        exclude_list=$exclude_list,212,213	# pluto sqlcvs
		                        exclude_list=$exclude_list,598,599	# pluto tribunetv
		                        exclude_list=$exclude_list,468,469	# pluto vdr
		                        exclude_list=$exclude_list,466,467	# pluto vdr plugin
		                        exclude_list=$exclude_list,239,240	# pluto bi-dir comm lib
		                        exclude_list=$exclude_list,419,420	# pluto cddb identification
		                        exclude_list=$exclude_list,403,404	# ansi to html
		                        exclude_list=$exclude_list,760,761	# mjproxy
		                        exclude_list=$exclude_list,283,284	# pluto text to speech
					;;
			esac
			;;
	esac

	# Set version of packages to todays date, plus 00:19 as time
	Q="Update Version Set VersionName= concat('$Main_Version',substr(now()+0,3,6),'$SVNrevision') Where PK_Version = 1;"
	mysql $PLUTO_BUILD_CRED -D 'pluto_main_build' -e "$Q"

	create_version_h ${svn_dir} ${svn_branch_name} ${Main_Version} $SVNrevision

	# Compile the packages
	echo "\"${mkr_dir}/MakeRelease\" $make_jobs -a -R \"$SVNrevision\" $PLUTO_BUILD_CRED -O \"$out_dir\" -D 'pluto_main_build' -o \"$Distro_ID\" -r \"$RepositorySource\" -m 1 -K \"$exclude_list\" -s \"${svn_dir}/${svn_branch_name}\" -n / -d"
	arch=$arch "${mkr_dir}/MakeRelease" $make_jobs -a -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r "$RepositorySource" -m 1 -K "$exclude_list" -s "${svn_dir}/${svn_branch_name}" -n / -d || Error "MakeRelease failed"

	# Compile the private packages
	if [ "$svn_private_url" -a "$svn_private_user" -a "$svn_private_pass" ]
	then
		echo "\"${mkr_dir}/MakeRelease\" $make_jobs -a -R \"$SVNrevision\" $PLUTO_BUILD_CRED -O \"$out_dir\" -D 'pluto_main_build' -o \"$Distro_ID\" -r \"$RepositorySource\" -m 1108 -K \"$exclude_list\" -s \"${svn_dir}/${svn_branch_name}\" -n / -d"
		arch=$arch "${mkr_dir}/MakeRelease" $make_jobs -a -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r "$RepositorySource" -m 1108 -K "$exclude_list" -s "${svn_dir}/${svn_branch_name}" -n / -d || Error "MakeRelease failed on private packages"
	fi
}


DisplayMessage "*** STEP: Running MakeRelease"
trap 'Error "Undefined error in $0"' EXIT

build_main_debs

trap - EXIT
