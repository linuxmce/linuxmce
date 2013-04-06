#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/build-scripts/name-packages.sh
LC_ALL=C

set -e
#set -x

# set NUMCORES=X in /etc/lmce-build/builder.custom.conf to enable multi-job builds
if [ ! "$NUM_CORES" ] ; then NUM_CORES=1 ; fi
if [ "$NUM_CORES" -gt 1 ] ; then make_jobs="-j $NUM_CORES"; fi

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
					# mame - excluded due to long time for compilation.
					exclude_list=$exclude_list,682,683
                       			# videolan client
#		                         exlucde_list=$exclude_list,431,432
                       	                exclude_list=$exclude_list,$mkr_tira
#                                        exclude_list=$exclude_list,$mkr_usb_uirt_0038_package
                                        exclude_list=$exclude_list,$mkr_vloopback_driver_package
#                                        exclude_list=$exclude_list,672,674 #
					;;
			esac
			;;
		"raspbian")
                        #FIXME Hackozaurus for ubuntu-diskless-tools
                        mkdir -p /home/DisklessFS/
                        diskless_image_name="PlutoMD_Debootstraped-$flavor-$build_name-$arch.tar.bz2"
                        cp "${diskless_dir}/$diskless_image_name" /home/DisklessFS

                        case "${build_name}" in
                                "wheezy")
	                                Distro_ID="19"
	                                RepositorySource="23"
	                                Main_Version='2.0.0.46.'
	                                exclude_list=$exclude_list,$mkr_vloopback_driver_package

	                                # does not compile and/or is not compatible
					exclude_list=$exclude_list,498,499 # SimplePhone
	                                exclude_list=$exclude_list,676  # Game Library lmce-game-db - source incompatible??
	                                exclude_list=$exclude_list,406  # UpdateMedia - works if lmce-game-db is built
	                                exclude_list=$exclude_list,780  # LinuxMCE mediatagging
	                                exclude_list=$exclude_list,672  # Game Plugin
	                                exclude_list=$exclude_list,674  # Game Player
	                                exclude_list=$exclude_list,679  # Game Database
	                                exclude_list=$exclude_list,717  # MESS
	                                exclude_list=$exclude_list,680  # MAME SNAPS
	                                exclude_list=$exclude_list,681  # MAME metadata
	                                exclude_list=$exclude_list,683  # MAME
	                                exclude_list=$exclude_list,722  # LinuxMCE DPMS Monitor
	                                exclude_list=$exclude_list,133  # MythTV Player
	                                exclude_list=$exclude_list,134  # MythTV Plugin
	                                exclude_list=$exclude_list,726  # MythTV Scripts
	                                exclude_list=$exclude_list,685  # EnOceon TCM120 - code error
	                                exclude_list=$exclude_list,751  # HAI Control Program - needs Makefile?
	                                exclude_list=$exclude_list,768  # PLCBUS - code error
	                                exclude_list=$exclude_list,773  # EIB - missing header?
	                                exclude_list=$exclude_list,777  # 1-wire driver
	                                exclude_list=$exclude_list,785  # OLA - missing dep
	                                exclude_list=$exclude_list,794  # LMCE AirPlay Audio Player - missing dep
	                                exclude_list=$exclude_list,628  # MCE diskless tools - need to update for rpi debootstrap$
	                                exclude_list=$exclude_list,687  # LMCE Datalogger plugin - says 'source not compatible wi$
	                                exclude_list=$exclude_list,689  # LMCE Datalogger library - says missing libdataloger...
	                                exclude_list=$exclude_list,690  # Generic PC Workstation
	                                exclude_list=$exclude_list,696  # LMCE Screen Capture Camera
	                                exclude_list=$exclude_list,704  # Shoutcast Radio Plugin
	                                exclude_list=$exclude_list,710  # IRTrans Ethernet
	                                exclude_list=$exclude_list,712  # Chromoflex LED
	                                exclude_list=$exclude_list,714  # VistaICM2
	                                exclude_list=$exclude_list,717,716      # sdl_mess
	                                exclude_list=$exclude_list,743,742      # hulu plugin
	                                exclude_list=$exclude_list,749,748      # hai omni rs232
	                                exclude_list=$exclude_list,759,758      # pandora
	                                exclude_list=$exclude_list,792,791      # airplay plugin
	                                exclude_list=$exclude_list,796,795      # usb game pad

	                                exclude_list=$exclude_list,804  # LMCE Skins BlueCrystal

	                                # needs pre-compiled binary
	                                exclude_list=$exclude_list,$mkr_tira
	                                exclude_list=$exclude_list,453  # IRTrans Wrapper

        	                        # Not required for an MD
	                                exclude_list=$exclude_list,211  # Pluto System database
	                                exclude_list=$exclude_list,214  # Pluto Local database
	                                exclude_list=$exclude_list,233  # Pluto media database
	                                exclude_list=$exclude_list,256  # Pluto security database
	                                exclude_list=$exclude_list,436  # Pluto Marbella database
	                                exclude_list=$exclude_list,454  # Pluto Audi database
	                                exclude_list=$exclude_list,472  # Pluto telecom database
	                                exclude_list=$exclude_list,679  # Pluto game database
					;;
			esac
			;;
	esac

	# Set version of packages to todays date, plus 00:19 as time
	Q="Update Version Set VersionName= concat('$Main_Version',substr(now()+0,3,6),'$SVNrevision') Where PK_Version = 1;"
	mysql $PLUTO_BUILD_CRED -D 'pluto_main_build' -e "$Q"

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
