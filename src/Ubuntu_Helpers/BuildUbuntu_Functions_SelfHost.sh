#!/bin/bash -x

source $SVNROOT/src/BootScripts/Utils.sh.prep
source $SVNROOT/settings.sh

export KDE_LMCE=""

flavor="ubuntu"
if [[ "$BITS"x == "64"x ]] ; then
	CONF=${SVNROOT}/src/Ubuntu_Helpers/${flavor}64.conf
else
	CONF=${SVNROOT}/src/Ubuntu_Helpers/${flavor}32.conf
fi
#echo MakeRelease_PrepFiles configuation file: $CONF
PK_DISTRO=$(grep ^pk_distro $CONF | awk '{ split($0, list, "="); print list[2] }')
RELEASE_VERSION=$(grep ^release_version $CONF | awk '{ split($0, list, "="); print list[2] }')
ADMINSITE_NAME=$(grep ^adminsite_name $CONF | awk '{ split($0, list, "="); print list[2] }')
DISTRO=$(grep ^distro $CONF | awk '{ split($0, list, "="); print list[2] }')
ARCH=$(grep ^arch $CONF | awk '{ split($0, list, "="); print list[2] }')
CORE_NAME=$(grep ^core_name $CONF | awk '{ split($0, list, "="); print list[2] }')
MAIN_URL=$(grep ^main_url $CONF | awk '{ split($0, list, "="); print list[2] }')
NAME_MIXED=$(grep ^name_mixed $CONF | awk '{ split($0, list, "="); print list[2] }')
NAME_LOWER=$(grep ^name_lower $CONF | awk '{ split($0, list, "="); print list[2] }')
NAME_UPPER=$(grep ^name_upper $CONF | awk '{ split($0, list, "="); print list[2] }')
FLAVOR_NAME=$(grep ^flavor_name $CONF | awk '{ split($0, list, "="); print list[2] }')
REPLACEMENTSDEB_DEBIAN=$(grep ^replacementsdeb_debian $CONF | awk '{ split($0, list, "="); print list[2] }')
MAINDEB_DEBIAN=$(grep ^maindeb_debian $CONF | awk '{ split($0, list, "="); print list[2] }')
REPLACEMENTSDEB_UBUNTU=$(grep ^replacementsdeb_ubuntu $CONF | awk '{ split($0, list, "="); print list[2] }')
MAINDEB_UBUNTU=$(grep ^maindeb_ubuntu $CONF | awk '{ split($0, list, "="); print list[2] }')

if [[ $build_dir == "" || $local_mirror_dir == "" ]] ;then
	build_dir="/var/plutobuild"
	local_mirror_dir="/var/www"
fi

if [[ "$iso_name" == "" ]] ;then
	iso_name="lmce-private"
fi

if [[ "$build_log" == "" ]] ; then
	build_log="/var/log/build.log"
fi

svn_url="http://10.0.2.4/"

sql_master_host="10.0.2.4"
sql_master_db="pluto_main"
sql_master_db_media="pluto_media"
sql_master_db_security="pluto_security"
sql_master_db_telecom="pluto_telecom"
sql_master_user="root"

# do not change the following to "localhost", if you do mysql will use
# the /var/run/mysqld/mysqld.sock socket and connect to the wrong DB!
sql_slave_host="127.0.0.1"
sql_slave_db="pluto_main_build"
sql_slave_db_mainsqlcvs="main_sqlcvs_$flavor"
sql_slave_db_mythsqlcvs="myth_sqlcvs_$flavor"
sql_slave_db_media="pluto_media"
sql_slave_db_security="pluto_security"
sql_slave_db_telecom="pluto_telecom"
#sql_slave_user="root"

replacements_dir="${build_root_dir}/packages/replacements"
out_dir="${build_dir}/out"
mkr_dir="${build_dir}/MakeRelease"

#disabled, can't do this until DB initialized -- dtk Oct 18th, 2007
#export Version=$(echo "select VersionName from Version" | mysql $sql_slave_db | tail -1);
function LoadPackageNames
{
	export mkr_shift_key_state_source=28
	export mkr_0_99=28,274

	export mkr_standard_plugins_package=116
	export mkr_xine_player_package=117
	export mkr_xine_plugin_package=118
	export mkr_orbiter_package=119
	export mkr_disk_drive_package=122
	export mkr_bluetooth_dongle_package=124
	export mkr_app_server_package=126
	export mkr_mythtv_player_package=133
	export mkr_mythtv_plugin_package=134
	export mkr_standard_plugins_source=135
	export mkr_xine_player_source=136
	export mkr_xine_plugin_source=137
	export mkr_orbiter_source=138
	export mkr_disk_drive_source=141
	export mkr_bluetooth_dongle_source=143
	export mkr_app_server_source=145
	export mkr_mythtv_player_source=152
	export mkr_mythtv_plugin_source=153
	export mkr_dce_router_package=154
	export mkr_dce_router_source=155
	export mkr_gc100_source=161
	export mkr_gc100_package=162
	export mkr_serial_class_library_source=190
	export mkr_serial_class_library_package=191
	export mkr_dce_library_source=192
	export mkr_dce_library_package=193
	export mkr_irbase_library_source_package=194
	export mkr_irbase_library_package=195
	export mkr_plutoutils_library_source=196
	export mkr_plutoutils_library_package=197
	export mkr_serializeclass_library_source=198
	export mkr_vipcommon_library_source=199
	export mkr_vipcommon_library_package=200

	export mkr_100_200=116,119,122,124,126,133,135
	export mkr_100_200=$mkr_100_200,138,141,143,145,152,153,154,155,161,162
	export mkr_100_200=$mkr_100_200,190,191,192,193,194,195,196,197
	export mkr_100_200=$mkr_100_200,199,200

	export mkr_pluto_main_library_source=201
	export mkr_sql2cpp_source=202
	export mkr_sql2cpp_package=203
	export mkr_skins_basic_package=204
	export mkr_development_dependencies_package=206
	export mkr_system_database_package=211
	export mkr_sqlcvs_source=212
	export mkr_sqlcvs_package=213
	export mkr_local_database_package=214
	export mkr_web_site_support_package=219         # disabled in All (pluto only)
	export mkr_web_site_admin_package=220           # disabled in All (pluto only)
	export mkr_orbitergen_package=226
	export mkr_orbitergen_source=227
	export mkr_voicemail_monitor_package=229
	export mkr_media_database_package=233
	export mkr_boot_scripts_package=234
	export mkr_pluto_main_library_package=237
	export mkr_serializeclass_library_package=238
	export mkr_bi_directional_comm_library_bd_source=239
	export mkr_bi_directional_comm_library_bd_package=240
	export mkr_messagesend_source=241
	export mkr_messagesend_package=242
	export mkr_request_action_ra_library_package=247
	export mkr_request_action_ra_library_source=248
	export mkr_pluto_media_library_source=254
	export mkr_pluto_media_library_package=255
	export mkr_security_database_package=256
	export mkr_diskless_base_filesystem_package=266
	export mkr_asterisk_package=270
	export mkr_asterisk_source=271
	export mkr_confirm_dependencies_package=272
	export mkr_shift_key_state_package=274
	export mkr_pluto_security_library_package=277
	export mkr_pluto_security_library_source=278
	export mkr_cm11a_source=279
	export mkr_cm11a_package=280
	export mkr_confirm_dependencies_source=281
	export mkr_text_to_speech_package=283
	export mkr_text_to_speech_source=284
	export mkr_slim_server_streamer_source=289
	export mkr_slim_server_streamer_package=290
	export mkr_qos_package=294
	export mkr_launch_manager_source=298
	export mkr_launch_manager_package=299

	export mkr_201_299=201,202,203,204,206,212,213
	export mkr_201_299=$mkr_201_299,226,227,234,237,198,238,239,240
	export mkr_201_299=$mkr_201_299,241,242,247,248,254,255,256,266,270,271
	export mkr_201_299=$mkr_201_299,272,277,278,279,280,281
	export mkr_201_299=$mkr_201_299,283,284,289,290,294,298,299

	export mkr_dhcpd_plugin_package=302
	export mkr_createdevice_package=303
	export mkr_createdevice_source=304
	export mkr_generic_serial_device_package=307
	export mkr_convert_mac_source=315
	export mkr_convert_mac_package=316
	export mkr_eib_package=318
	export mkr_eib_source=319
	export mkr_plutodhcp_source=330
	export mkr_linux_utility_packages_package=331
	export mkr_text_splitter_wrapper_source=333
	export mkr_mythtv_includes_package=334
	export mkr_generic_serial_device_source=335
	export mkr_voicemail_monitor_source=339
	export mkr_lirc_dce_wrapper_package=340
	export mkr_updateentarea_package=341
	export mkr_updateentarea_source=342
	export mkr_sdl_helpers_package=346
	export mkr_sdl_helpers_source=347
	export mkr_orbiterinstallers_package=363
	export mkr_windows_ce_libraries_package=364
	export mkr_install_scripts_package=391
	export mkr_remoteassistance_source=392
	export mkr_remoteassistance_package=393
	export mkr_dcegen_source=396
	export mkr_dcegen_package=397

	export mkr_300_399=302,303,304,307,315,316,318,319,331
	export mkr_300_399=$mkr_300_399,334,335,229,339,341,342,346,347
	export mkr_300_399=$mkr_300_399,363,364,391,392,393,396,397

	export mkr_messagetranslation_library_source_package=398
	export mkr_messagetranslation_library_package=401
	export mkr_ansi_to_html_source=403
	export mkr_ansi_to_html_package=404
	export mkr_updatemedia_source=405
	export mkr_updatemedia_package=406
	export mkr_database_settings_package=407
	export mkr_cddb_identification_source=419
	export mkr_cddb_identification_package=420
	export mkr_lirc_dce_wrapper_source=421
	export mkr_sample_media_package=422
	export mkr_slimserver_plugin_source=424
	export mkr_slimserver_plugin_package=425
	export mkr_videolan_plugin_server_source=429  # disabled in All (not used)
	export mkr_videolan_plugin_server_package=430 # disabled in All (not used)
	export mkr_videolan_plugin_client_source=431  # disabled in All (broken)
	export mkr_videolan_plugin_client_package=432 # disabled in All (broken)
	export mkr_marbella_database_package=436      # disabled in All (pluto only)
	export mkr_makerelease_source=446
	export mkr_makerelease_package=447
	export mkr_skins_rse_package=448
	export mkr_irtrans_source=452
	export mkr_irtrans_package=453
	export mkr_audi_database_package=454          # disabled in All (pluto only)
	export mkr_motion_wrapper_source=455
	export mkr_motion_wrapper_package=456
	export mkr_ati_video_drivers_package=462      # disabled in All (not supported)
	export mkr_tira_wrapper_source=464
	export mkr_tira_wrapper_package=465
	export mkr_vdr_plugin_source=466
	export mkr_vdr_plugin_package=467
	export mkr_vdr_source=468
	export mkr_vdr_package=469
	export mkr_pluto_telecom_library_source=470
	export mkr_pluto_telecom_library_package=471
	export mkr_telecom_database_package=472
	export mkr_skins_bo_package=473
	export mkr_proxy_orbiter_source=487
	export mkr_proxy_orbiter_package=488
	export mkr_nvidia_video_drivers_package=490
	export mkr_plutodhcp_package=491
	export mkr_powerfile_c200_source=492
	export mkr_powerfile_c200_package=493
	export mkr_disk_drive_functions_source=496
	export mkr_disk_drive_functions_package=497
	export mkr_simplephone_source=498
	export mkr_simplephone_package=499

	export mkr_398_499=398,401,403,404,405,406,407,419,420,340,421,422,424,425
	export mkr_398_499=$mkr_398_499,446,447,448,455,456
	export mkr_398_499=$mkr_398_499,464,465,466,467,468,469,470,471
	export mkr_398_499=$mkr_398_499,472,473,487,488,490,330,491,492,493
	export mkr_398_499=$mkr_398_499,496,497,498,499

	export mkr_monster_database_package=502       # disabled in All (pluto only)
	export mkr_network_storage_package=503 # delete "(51)" from Makefile..
	export mkr_skins_monster_package=504
	export mkr_zwave_lighting_source=505
	export mkr_zwave_lighting_package=506
	export mkr_network_storage_source=508
	export mkr_monster_nucleus_package=509
	export mkr_pwc_camera_drivers_source=515      # disabled in All (pluto only)
	export mkr_pwc_camera_drivers_package=516     # disabled in All (pluto only)
	export mkr_x_configuration_scripts_and_programs_package=520
	export mkr_dummy_pluto_console_utilities_package=522
	export mkr_x_configuration_scripts_and_programs_source=523
	export mkr_usb_uirt_0038_source=525
	export mkr_usb_uirt_0038_package=526
	export mkr_media_player_only_package=528
	export mkr_hal_daemon_source=529
	export mkr_hal_daemon_package=530
	export mkr_storage_devices_package=533
	export mkr_hal_device_finder_package=535
	export mkr_hak_device_finder_source=536
	export mkr_newmd_interactor_server_source=538
	export mkr_newmd_interactor_server_package=539
	export mkr_diskless_default_boot_file_package=540
	export mkr_vloopback_driver_source=542        # disabled in All (abandoned)
	export mkr_vloopback_driver_package=543       # disabled in All (abandoned)
	export mkr_libresolution_source=544
	export mkr_libresolution_package=545
	export mkr_disk_monitor_package=554
	export mkr_av_wizard_source=555
	export mkr_av_wizard_package=556
	export mkr_pnp_detection_scripts_package=562
	export mkr_test_serial_port_source=563
	export mkr_test_serial_port_package=564
	export mkr_av_wizard_skin_basic_package=567
	export mkr_av_wizard_skin_monster_package=568 # disabled in All (pluto only)
	export mkr_add_software_source=572
	export mkr_add_software_package=573
	export mkr_dynamic_dns_package=576
	export mkr_firewire2video4linux_package=578
	export mkr_sound_card_setup_scripts_package=579
	export mkr_capture_card_setup_scripts_package=580
	export mkr_photo_screen_saver_source=581
	export mkr_photo_screen_saver_package=582
	export mkr_wavetrend_reader_source=583        # disabled in All (abandoned)
	export mkr_wavetrend_reader_package=584       # disabled in All (abandoned)
	export mkr_alsa_drivers_source=589            # disabled in All (depreciated)
	export mkr_alsa_drivers_package=590           # disabled in All (depreciated)
	export mkr_tribunetv_source=598
	export mkr_tribunetv_package=599

	export mkr_500_599=503,504,505,506,508,509,520,522,523
	export mkr_500_599=$mkr_500_599,525,526,528,529,530,533,535,536
	export mkr_500_599=$mkr_500_599,538,539,540,544,545,554,555,556
	export mkr_500_599=$mkr_500_599,562,563,564,567,568,572,573,576,578,573
	export mkr_500_599=$mkr_500_599,576,578,579,580,581,582
	export mkr_500_599=$mkr_500_599,598,599

	export mkr_ati_video_drivers_source=607       # disabled in All (not supported)
	export mkr_raid_tools_package=617
	export mkr_hdhomerun_source=620
	export mkr_hdhomerun_package=621
	export mkr_mce_diskless_tools_source=627
	export mkr_mce_diskless_tools_package=628
	export mkr_xml_data_plugin_source=639
	export mkr_xml_data_plugin_package=640
	export mkr_amg_media_identifier_package=644   # disabled in All (abandoned)
	export mkr_amg_media_identifier_source=645    # disabled in All (abandoned)
	export mkr_mysql_wrapper_source=648
	export mkr_mysql_wrapper_package=649
	export mkr_lmce_update_system_source=650
	export mkr_lmce_update_system_package=651
	export mkr_fiire_drivers_package=652
	export mkr_mcr_remote_source=653
	export mkr_mcr_remote_package=654
	export mkr_mplayer_player_source=656
	export mkr_mplayer_player_package=657
#617
	export mkr_600_699=620,621,627,628,639,640,648,649
	export mkr_600_699=$mkr_600_699,650,651,652,653,654,656,657

	export mkr_ivtv_video_drivers_source=336
	export mkr_ivtv_video_drivers_package=337
	export mkr_videolan_plugin_common_source=426   # disabled in All (not used)
	export mkr_videolan_plugin_common_package=427  # disabled in All (not used)
	export mkr_ivtv_video_drivers=336,337
	export mkr_videolan_plugin_common=426,427
	export mkr_videolan_plugin_server=429,430
	export mkr_videolan_plugin_client=431,432
	export mkr_irtrans=452,453
	export mkr_pwc_camera_drivers=515,516
	export mkr_vloopback_driver=542,543
	export mkr_alsa_drivers=589,590
	export mkr_ati_video_drivers=462,607
	export mkr_launch_manager=298,299
	export mkr_wavetrend_reader=583,584
	export mkr_amg_media_identifier=644,645
	export mkr_usb_uirt=525,526
	export mkr_irbase=194,195
	export mkr_tira=464,465
	export mkr_mce_diskless_tools=627,628
	export mkr_dce_router=154,155
	export mkr_dce_library=192,193
	export mkr_zwave_lighting=505,506
	export mkr_xine_player=136,117
	export mkr_xine_plugin=137,118
	export mkr_mythtv_player=152,133
	export mkr_mythtv_plugin=153,134
	export mkr_serial=190,191
	export mkr_messagetranslation=398,401
	export mkr_standard_plugins=116,135
	export mkr_shift_key_state=28,274
	export mkr_simplephone=498,499
	export mkr_vdr_plugin=466,467
}

function DisplayMessage {
	echo "`date +%H:%M:%S`  $*" >&100
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
	local sync_pkgs="pluto-bluetooth-dongle pluto-cm11a pluto-eib pluto-gc100 pluto-generic-serial-device pluto-hdhomerun pluto-irbase pluto-irtrans-wrapper pluto-libbd pluto-lirc-wrapper pluto-messagetrans pluto-motion-wrapper pluto-msiml-disp-butt pluto-nvidia-video-drivers pluto-powerfile-c200 pluto-proxy-orbiter pluto-text-to-speech pluto-tira-wrapper pluto-usb-uirt-0038 pluto-vdr pluto-vipshared pluto-wavetrend-reader pluto-zwave-lighting mtx-pluto nvidia-glx lirc-pluto lirc-modules-2.6.22-14-generic"


	local pkg
	for pkg in $sync_pkgs ;do
		rm -f "/var/plutobuild/DisklessSync/${arch}/deb-cache/${pkg}_"*.deb
		cp "/var/www/${pkg}_"*.deb /var/plutobuild/DisklessSync/${arch}/deb-cache/
	done
}

function SUDO_Install_Build_Needed_Packages_SUDO
{
        if `test -f $SVNROOT/build/state_dev_packages_installed_1564` ; then
                exit 0
        fi

	firmware_src="deb http://dl.ivtvdriver.org/ubuntu feisty firmware"
	if [ ! -e /etc/apt/sources.list.pbackup ] ;then
		cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
	fi
	if ! grep -qF "$firmware_src" /etc/apt/sources.list; then
		local temp_file=$(mktemp)
		cp /etc/apt/sources.list $temp_file
		echo "$firmware_src" >> $temp_file
		sudo cp $temp_file /etc/apt/sources.list
	fi
	sudo apt-key add ${SVNROOT}/ubuntu/gpg/ivtvdriver.org.gpg

	echo "Retrieving updated package lists"
	sudo apt-get update > /dev/null

	#TODO:Get list outside the file as it started growing to big
	local pkgs="asterisk-dev autoconf automake automake1.10 autotools-dev build-essential cdw debhelper debootstrap dh-make dmsetup fakeroot fuse-utils g++ libaa1-dev libattr1-dev libavcodec-dev libavformat-dev libbluetooth2-dev libcaca-dev libcddb-dev libcdio-dev libcdparanoia0-dev libconfuse-dev libcurl3-dev libdancer-xml0-dev libdbus-1-dev libdbus-glib-1-dev libdirectfb-dev libdvdread-dev libesd0-dev libfuse-dev libglade2-dev libgnomevfs2-dev libgtk2.0-dev libhal-dev libhid-dev libid3-3.8.3-dev libjpeg62-dev liblinphone1-dev liblircclient-dev libmad0-dev libmediastreamer0-dev libmodplug-dev libmpcdec-dev libpostproc-dev libqt3-compat-headers libqt3-headers libqt3-mt-dev libsdl-gfx1.2-dev libsdl-image1.2-dev libsdl-sge-dev libsdl-ttf2.0-dev libsdl1.2-dev libstdc++5 libsvga1-dev libtcltk-ruby libtheora-dev libupnp-dev libusb-dev libvte-dev libx11-dev libxcb-shape0-dev libxcb-shm0-dev libxcb-xv0-dev libxine-dev libxmu-headers libxrender-dev libxtst-dev mkisofs module-assistant mysql-client mysql-server nasm qt3-apps-dev qt3-assistant qt3-designer qt3-dev-tools qt3-dev-tools-compat qt3-dev-tools-embedded qt3-linguist qt3-qtconfig quilt ruby1.8-dev sgmltools-lite squashfs-tools subversion swig tcl8.4-dev transfig x-dev x11proto-core-dev x11proto-record-dev x11proto-xext-dev x11proto-xf86vidmode-dev xorg-dev libdvb-dev libavc1394-dev liblame-dev libiec61883-dev libdvdnav-dev libimlib2-dev texi2html ccache libtag1-dev"
	# removed libhttpfetcher-dev since it no longer appears to be in the distribution
	# removed kdelibs4-dev libflac-dev libmagick9-dev libpulse-dev
	local pkg

        echo "Installing general development packages"
	for pkg in $pkgs ;do
		printf "."
		if ! [`sudo apt-get -y install $pkg > /dev/null`] ; then
                    echo "Error installing $pkg"
                fi
	done
        printf "\n"

        echo "Installing mysql stuff"
        sudo apt-get -y install libmysqlclient15-dev > /dev/null

        echo "Installing libxine development packages"
	pkgs="libcaca-dev liblircclient-dev libtheora-dev libflac-dev=1.1.2-5ubuntu2 libmodplug-dev libgnomevfs2-dev libspeex-dev libmad0-dev libxvmc-dev automake1.9 autoconf libtool libcdio-dev sgmltools-lite dpatch transfig libavformat-dev libpostproc-dev libavcodec-dev libraw1394-dev libdc1394-13-dev"
	for pkg in $pkgs ;do
		printf "."
		if ! [`sudo apt-get -y install $pkg > /dev/null`] ; then
                    echo "Error installing $pkg"
                fi
	done
        printf "\n"

        echo "Installing Linux headers for installed Linux release, needed for pluto-asterisk"
	sudo apt-get -y install linux-headers-`uname -r` > /dev/null

	echo "Installing lirc-source for installed Linux release"
	sudo apt-get -y install linux-source-`uname -r | cut -d'-' -f1` > /dev/null
	sudo apt-get -y install linux-headers-`uname -r` > /dev/null
	sudo apt-get -y install lirc-modules-source > /dev/null

	echo "ivtv-source (TODO still needed? -- dtk)"
	sudo apt-get -y install ivtv-source > /dev/null

        echo "Installing KDE development libraries 710"
        sudo apt-get -y install kdelibs4-dev libjasper-dev libpcre3-dev libpcrecpp0 kdebase-dev libarts1-dev libvorbis-dev libvorbis0a libvorbisenc2 libvorbisfile3 kdelibs4-dev > /dev/null

        echo "Removing libdvdnav-dev to make room for libxine version"
        sudo apt-get -y remove libdvdnav-dev > /dev/null
}

function Checkout_Pluto_Svn {
	echo "$(date) part 21 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
	local Branch="${1:-trunk}"
	local BranchParts="$2" # ex: "src web"; the rest will come from trunk

	local AllParts="src ubuntu web misc_utils installers config-pkgs"
	if [[ -z "$BranchParts" ]]; then
		BranchParts="$AllParts"
	fi

	[[ -d $svn_dir ]] && mkdir -p $svn_dir
	rm -rf ${svn_dir}/trunk
	
	for svn_module in ${BranchParts}; do
		mkdir -p ${svn_dir}/trunk/$svn_module
		#svn co -r 17898  ${svn_url}/pluto/"$Branch"/$svn_module  ${svn_dir}/trunk/$svn_module
		svn co  ${svn_url}/pluto/"$Branch"/$svn_module  ${svn_dir}/trunk/$svn_module
	done

	# get unmarked parts from trunk
	for svn_module in ${AllParts}; do
		if [[ " $BranchParts " == *" $svn_module "* ]]; then
			# this part was marked to be taken from the branch
			continue
		fi
		#get part from trunk
		mkdir -p ${svn_dir}/trunk/$svn_module
		#svn co -r 17898 ${svn_url}/pluto/trunk/$svn_module  ${svn_dir}/trunk/$svn_module
		svn co ${svn_url}/pluto/trunk/$svn_module  ${svn_dir}/trunk/$svn_module
	done

	cp -f /root/images-pluto-admin/*.jpg ${svn_dir}/trunk/web/pluto-admin/include/images/
	cp -f /root/images-pluto-admin/generic_xml_error_linuxmce.png ${svn_dir}/trunk/web/pluto-admin/security_images/generic_xml_error.png

	sed -i "s,\\\$wikiHost=.*\$,\$wikiHost='http://wiki.linuxmce.com/';,g" ${svn_dir}/trunk/web/plutohome-com/globalconfig/localconfig.inc.php

	#/bin/sql2cpp -h localhost -u root -D pluto_main
	pushd ${svn_dir}/trunk/src
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/ZWave/
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/Fiire_Scripts/
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/RFID_Interface/
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/EMI_AMG/
	svn co --username automagic --password "$(</etc/pluto/automagic.pwd)" ${svn_url}/pluto-private/"$Branch"/src/lmce_launch_manager/
	popd
	echo "$(date) part 22 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
}

function Build_MakeRelease_Binary_Install
{
        if `test -f $SVNROOT/build/state_make_release_binary_458` ; then
                exit 0
        fi

	mkdir -p ${mkr_dir}
	cp ${SVNROOT}/src/bin/MakeRelease ${mkr_dir}
	cp ${SVNROOT}/src/bin/MakeRelease_PrepFiles ${mkr_dir}
        rm -f ${mkr_dir}/mysql_wrapper
        ln -s /usr/bin/mysql ${mkr_dir}/mysql_wrapper
	cp ${SVNROOT}/src/lib/*.so ${mkr_dir}
}

function Build_MakeRelease_Binary {
	pushd ${SVNROOT}/src/pluto_main
	make
	popd

	pushd ${SVNROOT}/src/PlutoUtils
	SNR_CPPFLAGS="" make
	popd

	pushd ${SVNROOT}/src/SerializeClass
	SNR_CPPFLAGS="" make
	popd

	pushd ${SVNROOT}/src/DCE
	SNR_CPPFLAGS="" make
	popd

	pushd ${SVNROOT}/src/MakeRelease
	make -f Makefile.MakeRelease
	popd

	pushd ${SVNROOT}/src/MakeRelease_PrepFiles
	make
	popd

	pushd ${SVNROOT}/src/mysql_wrapper
	make
	popd

	mkdir -p ${mkr_dir}
#	mkdir -p /usr/pluto/bin
	cp ${SVNROOT}/src/bin/MakeRelease ${mkr_dir}
	cp ${SVNROOT}/src/bin/MakeRelease_PrepFiles ${mkr_dir}
        rm -f ${mkr_dir}/mysql_wrapper
        ln -s /usr/bin/mysql ${mkr_dir}/mysql_wrapper
	cp ${SVNROOT}/src/lib/*.so ${mkr_dir}
}

function Build_Replacement_Package
{
	pkg=("$1")
        pkgX=`echo $pkg | sed -e 's./._.g'`
        if `test -f $SVNROOT/build/state_build_replacement_$pkgX` ; then
                exit 0
        fi
	echo "Building package for $pkg"
	mkdir -p $replacements_dir > /dev/null
	pushd $pkg > /dev/null
	dpkg-buildpackage -rfakeroot -us -uc > /dev/null
	local ls_deb=`echo ../*.deb` > /dev/null
	if [[ "$ls_deb" != "" ]] ; then
		mv ../*.deb ${replacements_dir}
	fi
	popd > /dev/null
        mkdir -p ${SVNROOT}/build
        touch ${SVNROOT}/build/state_build_replacement_$pkgX
	echo
}

function Build_Replacement_Asterisk
{
	echo "Package: pluto-asterisk"
	mkdir -p $replacements_dir > /dev/null
	pushd ${SVNROOT}/ubuntu/asterisk
		./make_package_ubuntu.sh `uname -r` > /dev/null
		cp -r asterisk-pluto_*.deb ${replacements_dir}
	popd
	echo
}

function Build_Replacement_freepbx
{
	#Package: freepbx
	dir_="${svn_dir}/trunk/ubuntu/asterisk/freepbx"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp -r ../freepbx_*.deb ${temp_dir}
		popd
	fi
}

function SUDO_Build_LIRC_SUDO
{
	echo "Package: lirc-modules"
	mkdir -p $replacements_dir > /dev/null
	pushd . > /dev/null
                cd $SVNROOT/src/Ubuntu_Helpers
                sudo ./Preseed_lirc-modules-source.sh
                sudo rm -f /etc/lirc/lirc-modules-source.conf
                sudo dpkg-reconfigure -fnoninteractive lirc-modules-source

                echo "Unpacking Linux Source"
		pushd /usr/src > /dev/null
			sudo tar jxf linux-source-`uname -r | cut -d '-' -f1`.tar.bz2
		popd > /dev/null

                sudo mkdir -p /usr/src/modules/lirc/drivers/media/video/bt8xx/
                sudo cp -a /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video/bt8xx/* /lib/modules/`uname -r`/build/drivers/media/video
                sudo cp -a /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video/* /lib/modules/`uname -r`/build/drivers/media/video

                cd /usr/src/linux-source-`uname -r | cut -d '-' -f1`/drivers/media/video
                sudo cp -a btcx-risc.h /usr/src/modules/lirc/drivers/media/video
                cd /usr/src/modules/lirc/drivers
                echo "Building lirc-modules"
                sudo ln -s ../../../linux-source-`uname -r | cut -d'-' -f1`/drivers/video/ ./  || :
                sudo m-a -ft a-b lirc-modules
                cp /usr/src/lirc-modules*.deb "${replacements_dir}"
                sudo rm /usr/src/lirc-modules*.deb
	popd > /dev/null
	echo
}

function SUDO_Build_IVTV_SUDO
{
	echo "Package: ivtv-modules"
	mkdir -p $replacements_dir > /dev/null
	pushd . > /dev/null
		sudo m-a -ft a-b ivtv > /dev/null
		local who_am_i=`whoami`
                sudo chown $who_am_i /usr/src/ivtv-modules*.deb
		cp /usr/src/ivtv-modules*.deb "${replacements_dir}"
                sudo rm /usr/src/ivtv-modules*.deb
                echo
        popd > /dev/null
	echo
}

function Repackage_IVTV_Firmware
{
	echo "Package: ivtv-firmware"
	mkdir -p $replacements_dir > /dev/null
	pushd $SVNROOT/ubuntu/ > /dev/null
		$SVNROOT/src/Ubuntu_Helpers/RepackIvtvFirmware.sh > /dev/null
		cp ivtv-firmware_*.deb "${replacements_dir}"
		rm ivtv-firmware_*.deb
	popd > /dev/null
	echo
}

function Build_Replacement_LinuxDummyImage
{
	echo "Package: linux-image-dummy"
	mkdir -p $replacements_dir > /dev/null
	pushd "${SVNROOT}"/ubuntu/linux-image-dummy > /dev/null
		dpkg-deb -b . ..
                local ls_deb=`echo ../*.deb`
                if [[ "$ls_deb" != "" ]] ; then
                        mv ../*.deb ${replacements_dir}
                fi
	popd > /dev/null
	echo
}


function Build_liblibphone
{	
	#Package: liblinphone
	dir_="${svn_dir}/trunk/ubuntu/linphone-1.3.5"
	if ReplacementNeedsBuild "$dir_" ;then
		pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b
		cp ../liblinphone1-lmce_*.deb ${temp_dir}
		cp ../libortp4-lmce_*.deb ${temp_dir}
		popd
	fi
}

function Build_VideoWizardVideos
{
        if `test -f $SVNROOT/build/state_built_video_wizard_package_442` ; then
                exit 0
        fi

        mkdir -p $replacements_dir > /dev/null

	pushd "${SVNROOT}"/build/video-wizard-videos
	dpkg-deb -b . ..
	cp -r ../video-wizard-videos_*.deb ${replacements_dir}
	popd
}

function Build_zaptel_modules
{	
	#Package: zaptel-modules
	apt-get -y install zaptel-source
	m-a -ft -l 2.6.22-14-generic a-b zaptel
	cp /usr/src/zaptel-modules*.deb "${temp_dir}"
}

function Build_Pluto_Replacements {
	echo "$(date) part 1 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
       
	temp_dir="${replacements_dir}"
	mkdir -p $temp_dir > /dev/null

#	echo "Package: video-wizard-videos"
#	local vvv_temp_dir=$(mktemp -d)
#	pushd "$vvv_temp_dir"
#		scp -r pluto@10.0.2.4:"/home/samba/www_docs/video\ wizard/video-wizard-videos" ./
#		cd "video-wizard-videos"
#		dpkg-deb -b . ..
#		cp -r ../video-wizard-videos_*.deb ${replacements_dir}
#	popd
#	rm -rf "$vvv_temp_dir"

	local pkgs="misc_utils/tee-pluto ubuntu/xine-lib-1.1.3 ubuntu/mplayer ubuntu/lshwd-2.0-rc4"
	local pkg
	for pkg in $pkgs ; do
		Build_Replacement_Package $pkg
	done

	Build_Replacement_Asterisk
	SUDO_Build_LIRC_SUDO
	SUDO_Build_IVTV_SUDO

	echo "$(date) part 2 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log

        Repackage_IVTV_Firmware

	echo "$(date) part 3 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log

	local pkgs="ubuntu/lirc-pluto-0.1 ubuntu/mtx-1.3.11 src/mce-launcher src/mce-installer" 
	local pkg
	for pkg in $pkgs ; do
		Build_Replacement_Package $pkg
	done

        Build_Replacement_LinuxDummyImage
	
	#Download arch independent packages from 150
	pushd $temp_dir
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/libflickr-api-perl_1_all.deb ./
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/libxml-parser-lite-tree-perl_1_all.deb ./
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/asterisk-perl_0.08-1_all.deb ./
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/pluto-avwizard-sounds_1.0-1_all.deb ./
		scp  pluto@10.0.2.4:/home/samba/repositories/replacements-common/msttcorefonts_2.2pluto1_all.deb ./
	popd 
}

function Build_Pluto_Stuff_Compile {
	:
}

function Build_Pluto_Stuff_Debs {
	:
}

function Run_MakeRelease_Prep
{
        if `test -f $SVNROOT/build/state_ran_makerelease_prep` ; then
                exit 0
        fi

	echo "$(date) part 4 a" >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log

	export PATH=${mkr_dir}:$PATH:${SVNROOT}/src/bin
	export LD_LIBRARY_PATH="$mkr_dir:${SVNROOT}/src/lib"
	SVNrevision=$(svn info $SVNROOT/src |grep ^Revision | cut -d" " -f2)

	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	echo Running MakeRelease_PrepFiles
#	$MakeRelease_PrepFiles -p ${SVNROOT} -e "*.prep,*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" -c ${CONF}
	$MakeRelease_PrepFiles -p ${SVNROOT} -e "*.prep" -c ${CONF}

        mkdir -p ${SVNROOT}/build
        touch ${SVNROOT}/build/state_ran_makerelease_prep
}

function Run_MakeRelease_Clean
{
	rm -rf $out_dir
}

function Run_MakeRelease_All
{
        local db_user=($1)
        local db_password=($2)
        local db_port=($3)

	echo "$(date) part 4 b" >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
	
	export PATH=${mkr_dir}:$PATH:${SVNROOT}/src/bin

	export LD_LIBRARY_PATH="$mkr_dir:${SVNROOT}/src/lib"

	# Linphone 1.5.1 in Ubuntu Feisty makes
	# SimplePhone to need this
	export LINPHONE_CONST=const
	export PKG_CONFIG_PATH=/opt/linphone-1.3.5/lib/pkgconfig

	mkdir -p $out_dir

	LoadPackageNames

	source $SVNROOT/settings.sh

	exclude_list=$mkr_ivtv_video_drivers
	exclude_list=$exclude_list,$mkr_videolan_plugin_common
	exclude_list=$exclude_list,$mkr_videolan_plugin_server
	exclude_list=$exclude_list,$mkr_videolan_plugin_client
	exclude_list=$exclude_list,$mkr_pwc_camera_drivers
	exclude_list=$exclude_list,$mkr_vloopback_driver
	exclude_list=$exclude_list,$mkr_alsa_drivers
	exclude_list=$exclude_list,$mkr_ati_video_drivers
	exclude_list=$exclude_list,$mkr_wavetrend_reader
	exclude_list=$exclude_list,$mkr_av_wizard_skin_monster_package
	exclude_list=$exclude_list,$mkr_monster_database_package
	exclude_list=$exclude_list,$mkr_amg_media_identifier
	exclude_list=$exclude_list,$mkr_web_site_support_package
        exclude_list=$exclude_list,$mkr_web_site_admin_package
	exclude_list=$exclude_list,$mkr_marbella_database_package
	exclude_list=$exclude_list,$mkr_audi_database_package
	exclude_list=$exclude_list,$mkr_skins_rse_package
	exclude_list=$exclude_list,$mkr_skins_bo_package
	exclude_list=$exclude_list,$mkr_skins_monster_package

	echo "$(date) part 5 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log

	# Make sure we can find sqlCVS
	PATH=$PATH:/usr/pluto/bin

        echo Running MakeRelease
	MakeRelease="${mkr_dir}/MakeRelease"
	$MakeRelease -R "$SVNrevision" \
                     -h $sql_slave_host -u $db_user \
                     -p $db_password -P $db_port -L -j 2 \
                     -O $out_dir -D $sql_slave_db -o $PK_DISTRO -r 21 -m 1 \
                     -K "$exclude_list" \
                     -s "${SVNROOT}" -n "${SVNROOT}/build/tmp" > >(tee -a $build_dir/Build.log)
	echo "$(date) part 6 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
}

function Run_MakeRelease
{
        local db_user=($1)
        local db_password=($2)
        local db_port=($3)
        local tmpx="\$$4"
        LoadPackageNames
        local db_devices=`eval printf "%s" $tmpx`

	source ${SVNROOT}/settings.sh

	export PATH=${mkr_dir}:$PATH:${SVNROOT}/src/bin
#	export PATH=/usr/lib/ccache:$PATH:${SVNROOT}/src/bin
	export LD_LIBRARY_PATH="$mkr_dir:${SVNROOT}/src/lib"

	SVNrevision=$(svn info $SVNROOT/src |grep ^Revision | cut -d" " -f2)
	LinphoneVersion="$(dpkg -s 'liblinphone1' | grep '^Version: ' | cut -d' ' -f2)"
	if [[ "$LinphoneVersion" == "1.5."* ]]; then
		# Linphone 1.5.1 in Ubuntu Feisty makes
		# SimplePhone to need this
		export LINPHONE_CONST=const
	fi

	mkdir -p $out_dir

	# Make sure we can find sqlCVS
	PATH=$PATH:/usr/pluto/bin

        echo "Running MakeRelease for $db_devices"

	MakeRelease="${mkr_dir}/MakeRelease"
	$MakeRelease -R "$SVNrevision" \
                     -h $sql_slave_host -u $db_user \
                     -p $db_password -P $db_port -L -j 2 \
                     -O $out_dir -D $sql_slave_db -o $PK_DISTRO -r 21 -m 1 \
                     -k "$db_devices" \
                     -s "${SVNROOT}" -n "${SVNROOT}/build/tmp" > >(tee -a $build_dir/Build.log)
}

function Create_Fake_Windows_Binaries {
	echo "$(date) part 7 " >> $build_log
	ls -l ${SVNROOT}/src/bin/ >> $build_log

	touch ${SVNROOT}/src/bin/Pluto_S60.sis
	touch ${SVNROOT}/src/bin/Orbiter.CAB
	touch ${SVNROOT}/src/bin/Orbiter_Treo.CAB

	touch ${SVNROOT}/src/bin/UpdateBinary.exe
	touch ${SVNROOT}/src/bin/UpdateBinaryCE.exe 
	touch ${SVNROOT}/src/bin/UpdateBinaryCE_x86.exe
	touch ${SVNROOT}/src/bin/{Orbiter_Win32.dat,Orbiter_CeNet4_XScale.dat}
	touch "${SVNROOT}/src/bin/Symbian Series 60 mobile.vmc"
	touch "${SVNROOT}/src/bin/Windows Mobile Smartphone.vmc"
	touch ${SVNROOT}/src/bin/Orbiter_CeNet4_x86.dat

	touch ${SVNROOT}/src/bin/OrbiterInstaller.msi
	touch ${SVNROOT}/src/bin/OrbiterCE_SDL.CAB
	touch ${SVNROOT}/src/bin/Orbiter_CeNet4_XScale.CAB
	touch ${SVNROOT}/src/bin/Orbiter_CeNet4_x86.CAB
	touch ${SVNROOT}/src/bin/ImportContacts.zip

	touch ${SVNROOT}/src/bin/Orbiter.exe
	touch ${SVNROOT}/src/bin/Orbiter_CeNet4_XScale.exe
	touch ${SVNROOT}/src/bin/OrbiterSmartphone.exe
	touch ${SVNROOT}/src/bin/OrbiterCE_SDL.exe
	touch ${SVNROOT}/src/bin/Orbiter_CeNet4_x86.exe

	touch ${SVNROOT}/src/bin/AYGSHELL.DLL
	touch ${SVNROOT}/src/bin/PthreadsCE.dll
	touch ${SVNROOT}/src/bin/Orbiter.MD5
	touch ${SVNROOT}/src/bin/logo.gif

	touch ${SVNROOT}/src/bin/PlutoBaSInstaller.msi
	touch ${SVNROOT}/src/bin/PlutoRebootSetup.msi


#	pushd ${SVNROOT}/src/bin
#	scp pluto@10.0.2.4:'/home/builds/Windows_Output_LinuxMCE/src/bin/*' ./
#	popd

#	pushd ${SVNROOT}/src/lib
#	scp pluto@10.0.2.4:'/home/builds/Windows_Output_LinuxMCE/src/lib/*' ./
#	popd
	
	echo "$(date) part 8 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
}

function Create_Local_Repository {
	echo "$(date) part 9 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
	
	rm -f $local_mirror_dir/*.deb
	rm -f $local_mirror_dir/Packages*

	mkdir -p $local_mirror_dir

	cp ${out_dir}/tmp/*.deb $local_mirror_dir
	cp ${replacements_dir}/*.deb $local_mirror_dir
	pushd $local_mirror_dir
		dpkg-scanpackages . /dev/null > Packages
		cat Packages | gzip -9c > Packages.gz
	popd

	echo "$(date) part 10 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
}

function SUDO_Backup_Database_SUDO
{
    echo "Backing up Database"
    pushd /var/lib
    sudo /etc/init.d/mysql stop
    local temp_file=$(mktemp).tbz
    sudo tar cfj $temp_file mysql
    sudo /etc/init.d/mysql start
    cp $temp_file $SVNROOT/build/db-backup-`date "+%Y-%m-%d-%H%M%S"`.tbz
    sudo rm -f $temp_file
    popd
}

function SUDO_Restore_Database_SUDO
{
    echo "Restoring up Database"
    pushd /var/lib
    local backup_file=`ls $SVNROOT/build/db-backup-* | sort -k1 | tail -n1`
    if [[ "$backup_file" == "" ]] ; then
        echo "You must make a backup before I can restore one.."
        exit 1
    fi
    sudo /etc/init.d/mysql stop
    local old_db=$(sudo mktemp -d /var/lib/mysql_old_XXXXXX)
    sudo mv mysql $old_db
    sudo tar xfj $backup_file
    sudo /etc/init.d/mysql start
    popd
}

function SUDO_Create_Build_Database_SUDO
{
    if `test -f /etc/init.d/mysql-lmce-build` ; then
        return 0
    fi
    echo "Creating Build Database"

    # First create a tar file
    local tmp=$(mktemp -d /tmp/mysql_scripts_XXXXXX)
    echo tmp: \"$tmp\"
    cp -R $SVNROOT/src/Ubuntu_Helpers/mysql-lmce-build-scripts/* $tmp
    pushd $tmp
        rm -Rf `find . -name .svn -print`
        chmod 755 $tmp $tmp/etc $tmp/etc/init.d $tmp/usr $tmp/usr/bin 
        tar cf $SVNROOT/build/mysql-lmce-build.tar .
    popd
    rm -Rf $tmp

    # Now extract the tar file, setting the owner to root
    pushd /
        sudo tar xfo $SVNROOT/build/mysql-lmce-build.tar
    popd

    # Now copy regular db store to the build db store as initial db
    sudo /etc/init.d/mysql stop
    sudo killall mysqld
    sleep 5
    sudo rm -Rf /var/run/mysqld-lmce-build
    sudo rm -Rf /var/lib/mysql-lmce-build
    sudo cp -Rp /var/lib/mysql /var/lib/mysql-lmce-build
    sudo /etc/init.d/mysql start
}

function SUDO_Start_Build_Database_SUDO
{
    if ! `test -f /var/run/mysqld-lmce-build/mysqld.pid` ; then
        echo "Starting Build Database"
        sudo /etc/init.d/mysql-lmce-build restart
    fi
}

function SUDO_Init_Build_Database_Permissions_SUDO
{
    if `test -f $SVNROOT/build/state_db_permissions_458` ; then
        return 0
    fi
    echo "Initializing Build Database"

    local db_user=("$1")
    local db_password=("$2")
    local db_port=("$3")

    if [[ "$db_user" == "" ]] ; then
        echo "Can not add blank user to DB"
        exit 1
    fi
    local hostparam=$sql_slave_host
    if [[ "$hostparam" == "" ]] ; then
        hostparam="%"
    fi

    echo "Adding a $db_user to mysql user database and granting permissions"

    local temp_file=$(mktemp)
    echo "GRANT ALL ON *.* to '$db_user'@'$hostparam'
          IDENTIFIED BY '$db_user'; " > $temp_file
    echo "SET PASSWORD FOR 
          '$db_user'@'$hostparam'=PASSWORD('$db_password');" >> $temp_file
    echo "GRANT ALL ON *.* to '$db_user'@'$hostparam'
          IDENTIFIED BY '$db_user'; " >> $temp_file

    echo "Adding a debian-sys-maint user to mysql user database and granting permissions"
    echo "GRANT ALL ON *.* to 'debian-sys-maint'@'localhost' IDENTIFIED BY '`grep 'password' /etc/mysql-lmce-build/debian.cnf | head -n 1 | cut -f3 -d' '`';" >> $temp_file

    echo "FLUSH privileges;" >> $temp_file
    sudo cat $temp_file | mysql -u root -h $sql_slave_host -P $db_port

    mkdir -p ${SVNROOT}/build
    touch ${SVNROOT}/build/state_db_permissions_458
}

function Init_Build_Database
{
	local db_user=("$1")
        local db_password=("$2")
        local db_port=("$3")
        local hostparam="-h $sql_slave_host -P $db_port"
        local mysqlcmd="mysql $hostparam -u $db_user -p$db_password"
	local sedcmd=$(echo "sed -e 's/<-mkr_t_pk_distro->/$PK_DISTRO/g' | sed -e 's/<-mkr_t_release_version->/$RELEASE_VERSION/g' | sed -e 's/<-mkr_t_adminsite_name->/$ADMINSITE_NAME/g' | sed -e 's/<-mkr_t-distro->/$DISTRO/g' | sed -e 's/<-mkr_t-arch->/$ARCH/g' | sed -e 's/<-mkr_t-core_name->/$CORE_NAME/g' | sed -e 's/<-mkr_t_main_url->/$MAIN_URL/g' | sed -e 's/<-mkr_t_name_mixed->/$NAME_MIXED/g' | sed -e 's/<-mkr_t_flavor_name->/$FLAVOR_NAME/g' | sed -e 's/<-mkr_t_replacementsdeb_debian->/$REPLACEMENTSDEB_DEBIAN/g' | sed -e 's/<-mkr_t_maindeb_debian->/$MAINDEB_DEBIAN/g' | sed -e 's/<-mkr_t_name_lower->/$NAME_LOWER/g' | sed -e 's/<-mkr_t_maindeb_ubuntu->/$MAINDEB_UBUNTU/g' | sed -e 's/<-mkr_t_replacementsdeb_ubuntu->/$REPLACEMENTSDEB_UBUNTU/g'")

	local UPDATE="update Package_Directory_File set MakeCommand='make -f Makefile.cvs; LDFLAGS=\"-L../../lib\" CPPFLAGS=\"-I../../ -I../../DCE -I../../Gen_Devices\" CXXFLAGS=\"-O0 -g3\" ./configure; make ; cp src/lmce_launch_manager ../bin/lmce_launch_manager ' where PK_Package_Directory_File=533;"

        if `test -f $SVNROOT/build/state_db_initialized_20501` ; then
		echo $UPDATE | $mysqlcmd $sql_slave_db
                return 0
        fi

	echo   "DROP DATABASE IF EXISTS $sql_slave_db;"              \
	       "DROP DATABASE IF EXISTS $sql_slave_db_mainsqlcvs;"   \
	       "DROP DATABASE IF EXISTS $sql_slave_db_mythsqlcvs;"   \
	       "DROP DATABASE IF EXISTS $sql_slave_db_media;"        \
	       "DROP DATABASE IF EXISTS $sql_slave_db_security;"     \
	       "DROP DATABASE IF EXISTS $sql_slave_db_telecom;"      \
	       "DROP DATABASE IF EXISTS pluto_main;"                 \
                   | $mysqlcmd  
	echo   "CREATE DATABASE $sql_slave_db;"                      \
	       "CREATE DATABASE $sql_slave_db_mainsqlcvs;"           \
	       "CREATE DATABASE $sql_slave_db_mythsqlcvs;"           \
	       "CREATE DATABASE $sql_slave_db_media;"                \
	       "CREATE DATABASE $sql_slave_db_security;"             \
	       "CREATE DATABASE $sql_slave_db_telecom;"              \
	       "CREATE DATABASE pluto_main;"                         \
                   | $mysqlcmd

        local db_cnt=6
        echo "Loading main build database 1/$db_cnt"
	eval "bzcat $SVNROOT/db_dumps/pluto_main_build.sql.bz2   | $sedcmd | $mysqlcmd $sql_slave_db"
        echo "Loading media database 2/$db_cnt"
	eval "bzcat $SVNROOT/db_dumps/pluto_media.sql.bz2        | $sedcmd | $mysqlcmd $sql_slave_db_media"
        echo "Loading security database 3/$db_cnt"
	eval "cat $SVNROOT/db_dumps/pluto_security.sql           | $sedcmd | $mysqlcmd $sql_slave_db_security"
        echo "Loading telecom database 4/$db_cnt"
	eval "cat $SVNROOT/db_dumps/pluto_telecom.sql            | $sedcmd | $mysqlcmd $sql_slave_db_telecom"
        echo "Loading mythtv database 5/$db_cnt"
	eval "cat $SVNROOT/db_dumps/myth_sqlcvs_ubuntu.sql       | $sedcmd | $mysqlcmd $sql_slave_db_mythsqlcvs"
        echo "Loading mythtv database 6/$db_cnt"
	eval "bzcat $SVNROOT/db_dumps/main_sqlcvs_ubuntu.sql.bz2 | $sedcmd | $mysqlcmd $sql_slave_db_mainsqlcvs"

#	cat $temp_file | $mysqlcmd "pluto_main"
#	cat $temp_file_main | $mysqlcmd $sql_slave_db_mainsqlcvs
#	cat $temp_file_myth | $mysqlcmd $sql_slave_db_mythsqlcvs

	echo $UPDATE | $mysqlcmd $sql_slave_db

	export Version=$(echo "select VersionName from Version" | $mysqlcmd $sql_slave_db | tail -1);
        echo "Current DB Version is $Version"

        mkdir -p ${SVNROOT}/build
        touch ${SVNROOT}/build/state_db_initialized_20501
}

function SUDO_Setup_Build_Database_SUDO
{
        if `test -f $SVNROOT/build/state_db_setup_20501` ; then
                return 0
        fi

	local DB_USER=($1)
        local DB_PASSWD=($2)
        local DB_PORT=($3)

	mkdir -p build

        SUDO_Backup_Database_SUDO
	SUDO_Create_Build_Database_SUDO
	SUDO_Start_Build_Database_SUDO
	SUDO_Init_Build_Database_Permissions_SUDO $DB_USER $DB_PASSWD $DB_PORT
	Init_Build_Database $DB_USER $DB_PASSWD $DB_PORT
}

function Import_Build_Database {
	local temp_sqlcvsdir=$(mktemp -d)
	local temp_file=$(mktemp)
	local temp_file_main=$(mktemp)
	local temp_file_myth=$(mktemp)
	local temp_file_media=$(mktemp)
	local temp_file_security=$(mktemp)
	local temp_file_telecom=$(mktemp)

	## Import sqlcvs repositories from plutohome.com
	ssh -i /root/.ssh/uploads_plutohome_key uploads@plutohome.com "
                set -x;
                rm -f /tmp/main_sqlcvs.dump /tmp/myth_sqlcvs /home/uploads/sqlcvs_dumps.tar.gz;
                mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata main_sqlcvs > /tmp/main_sqlcvs.dump;
                mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata myth_sqlcvs > /tmp/myth_sqlcvs.dump;
                cd /tmp;
                tar zcvf /home/uploads/sqlcvs_dumps.tar.gz main_sqlcvs.dump myth_sqlcvs.dump"
        scp -i /root/.ssh/uploads_plutohome_key uploads@plutohome.com:/home/uploads/sqlcvs_dumps.tar.gz $temp_sqlcvsdir
        pushd $temp_sqlcvsdir
	        tar zxvf sqlcvs_dumps.tar.gz
		mv main_sqlcvs.dump $temp_file_main
		mv myth_sqlcvs.dump $temp_file_myth 
	popd
	## Import other databases from 150
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db > $temp_file        
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_media > $temp_file_media
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_security > $temp_file_security
	mysqldump -h $sql_master_host -u $sql_master_user $sql_master_db_telecom > $temp_file_telecom

	export LD_LIBRARY_PATH="$mkr_dir:${SVNROOT}/src/lib"
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
	#echo 'update Package_Directory SET FK_Distro = NULL WHERE PK_Package_Directory = 675' | mysql -h $sql_slave_host -u $sql_slave_user $sql_slave_db

	rm -rf $temp_file $temp_file_main $temp_file_myth $temp_file_media $temp_file_security $temp_file_telecom $temp_sqlcvsdir

	#export Version=$(echo "select VersionName from Version" | mysql $sql_slave_db | tail -1);
}

function Import_Pluto_Skins {
	echo "$(date) part 11 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log

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
	
	echo "$(date) part 12 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log
}

function Create_Diskless_Archive
{
        if `test -f $SVNROOT/build/state_diskless_archive_created_1333` ; then
                exit 0
        fi

	echo "$(date) part 13 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log

	local temp_dir=$(mktemp -d /tmp/create_diskless_archive_XXXXXX)
        local src_repo=http://ro.archive.ubuntu.com/ubuntu/
        echo "Retrieving packages for bootstrap, this can take a while."
	sudo /usr/sbin/debootstrap feisty $temp_dir $src_repo > /dev/null

        echo "Creating bzipped tar of bootstrap filesystem."
	local who_am_i=`whoami`
	local temp_file=$(mktemp /tmp/debian_bootstrap_XXXXXX)
	pushd $temp_dir > /dev/null
		sudo tar -jcf $temp_file *
		sudo chown $who_am_i $temp_file
	popd > /dev/null
	sudo rm -rf $temp_dir

        local disklessfs=$build_root_dir/DisklessFS
	mkdir -p $disklessfs/BootWait
	mv $temp_file $disklessfs/PlutoMD_Debootstraped.tar.bz2

	touch $disklessfs/ramdisk.tar.bz2
	touch $disklessfs/PlutoMD.tar.bz2
	touch $disklessfs/BootWait/vmlinuz-default-2.6.16.20-pluto-2-686-smp
	touch $disklessfs/BootWait/initrd.img-default-2.6.16.20-pluto-2-686-smp
	
	echo "$(date) part 14 " >> $build_log
	ls -l $SVNROOT/src/bin/ >> $build_log

}

function Create_ISO {
	# Create the iso
	export local_cache_cd=$build_dir/cache-cd
	pushd $local_mirror_dir
                $SVNROOT/src/Ubuntu_Helpers/cd1-build.sh $iso_name
#	        $SVNROOT/src/Ubuntu_Helpers/cd2-build.sh $iso_name
	popd
}

function Upload_Build_Archive {
	pushd $local_mirror_dir
		tar -zcf /var/plutobuild/linuxmce-uploads.tar.gz *.iso
		scp -i /root/.ssh/uploads_plutohome_key /var/plutobuild/linuxmce-uploads.tar.gz uploads@plutohome.com:
#	scp -i /root/.ssh/uploads_linuxmce_build_150_key /var/plutobuild/linuxmce-uploads.tar.gz uploads@deb.plutohome.com:
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
