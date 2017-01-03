#!/bin/bash

set -e

. /usr/pluto/bin/Network_Parameters.sh 2>/dev/null 
. /usr/pluto/bin/Config_Ops.sh 2>/dev/null
. /usr/pluto/bin/Section_Ops.sh 2>/dev/null
. /usr/pluto/bin/LockUtils.sh 2>/dev/null
. /usr/pluto/bin/Utils.sh 2>/dev/null

DEVICEDATA_Extra_Parameters=139
DEVICEDATA_Extra_Parameters_Override=140
DEVICEDATA_Architecture=112
DEVICEDATA_DisklessBoot=9
DEVICEDATA_DisklessImages=258
DEVICEDATA_Model=233
DEVICEDATA_PK_Distro=7
DEVICEDATA_Serial_Number=161

DD_DISTRO_Raspbian_Wheezy=19
DD_DISTRO_Raspbian_Jessie=22

HOST_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
HOST_RELEASE=$(lsb_release -c -s)
HOST_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)

function setup_tftp_boot_rpi
{
	Moon_DistroID="$1"
	Moon_Serial_Number=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_Serial_Number'")

	local Moon_BootDir="/tftpboot/$Moon_Serial_Number"
	local Moon_cmdline_File="$Moon_BootDir/cmdline.txt"
	local Moon_config_File="$Moon_BootDir/config.txt"

	local BootConf=""

	ln -sf "/usr/pluto/diskless/$Moon_DeviceID/boot" "$Moon_BootDir"
	echo "dwc_otg.lpm_enable=0 console=serial0,115200 console=tty1 root=/dev/nfs nfsroot=192.168.80.1:/usr/pluto/diskless/$Moon_DeviceID rw ip=dhcp rootwait elevator=deadline" >"${Moon_cmdline_File}"

	cat <<-EEOF >"${Moon_config_File}"
		# For more options and information see
		# http://www.raspberrypi.org/documentation/configuration/config-txt.md
		# Some settings may impact device functionality. See link above for details

		# TODO: bring other values from the database to here

		# uncomment if you get no picture on HDMI for a default "safe" mode
		#hdmi_safe=1
		disable_overscan=1
		config_hdmi_boost=4
		hdmi_force_hotplug=1
		lcd_rotate=2
		gpu_mem=128

		# Enable audio (loads snd_bcm2835)
		dtparam=audio=on
		EEOF
}

function setup_tftp_boot 
{
	echo "* Configuring TFTP Boot for MD #${Moon_DeviceID}"

	Moon_DistroID=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_PK_Distro'")

	# For jessie we setup the native rpi pxe booting methods rather than standard pxe
	case $Moon_DistroID in
		$DD_DISTRO_Raspbian_Jessie)
			setup_tftp_boot_rpi $Moon_DistroID
			return 0
			;;
	esac

	local Moon_BootConfFile="/tftpboot/pxelinux.cfg/01-$(echo ${Moon_MAC//:/-} | tr 'A-Z' 'a-z')"
	local BootConf=""

	# deprecated boot parameter "video=" and we don't want to force vesafb with modern accelerated framebuffers (ie intel)
	# if extra parameters are necessary they should be added to the DT as DEVICEDATA_Extra_Parameters
	#local BootParams="quiet splash video=uvesafb:mode_option=1024x768-24,mtrr=3,scroll=ywrap vmalloc=256m"
	#local BootParams="quiet splash"
	local BootParams=""
	local BootParams_Extra=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $Moon_DeviceID AND FK_DeviceData = $DEVICEDATA_Extra_Parameters")
	local BootParams_Override=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $Moon_DeviceID AND FK_DeviceData = $DEVICEDATA_Extra_Parameters_Override")
	if [[ "$BootParams_Override" == "1" ]] ;then
		BootParams="${BootParams_Extra}"
	else
		BootParams="${BootParams} ${BootParams_Extra}"
	fi

	local DefaultBootName=

	mkdir -p /tftpboot/pxelinux.cfg

	# If a diskless image is specified it may have a specific required kernel,
	# set the pxeconfig according to the specified diskless image name, if provided
	if [[ -n "$Moon_DisklessImages" ]]; then
		for line in "$Moon_DisklessImages"; do
			NameKernel="${line%%=*}"
			if [[ "$NameKernel" == *-* ]]; then
				Name="${NameKernel%%-*}"
				Kernel="${NameKernel#*-}"
			else
				Name="$NameKernel"
				Kernel=
			fi
			[[ -z "$Kernel" ]] && continue || :
			[[ -z "$DefaultBootName" ]] && DefaultBootName="$Name" || :
			Value="${line#*=}"

			# Only create symlinks if $Name is defined.
			if [[ -n "$Name" ]]; then
				mkdir -p "/tftpboot/${Moon_DeviceID}/$Name"
				rm -f "/tftpboot/${Moon_DeviceID}/$Name/"{vmlinuz,initrd.img}
				if [[ ! -f "${Moon_RootLocation}/boot/vmlinuz-${Kernel}" ]]; then
					LC_ALL="C" chroot "${Moon_RootLocation}" apt-get install "linux-image-${Kernel}"
				fi

				# Changed to point to the always updated ubuntu softlink at device root. 
				ln -s "${Moon_RootLocation}/vmlinuz" "/tftpboot/${Moon_DeviceID}/$Name/vmlinuz"
				ln -s "${Moon_RootLocation}/initrd.img" "/tftpboot/${Moon_DeviceID}/$Name/initrd.img"
			fi
		done

		# Only create the pxeconfig if $DefaultBootName is defined.
		if [[ -n "$DefaultBootName" ]]; then
			BootConf="${BootConf}DEFAULT LinuxMCE\n"
			BootConf="${BootConf}LABEL LinuxMCE\n"
			BootConf="${BootConf}KERNEL ${Moon_DeviceID}/$DefaultBootName/vmlinuz\n"
			BootConf="${BootConf}APPEND initrd=${Moon_DeviceID}/$DefaultBootName/initrd.img ramdisk=10240 rw root=/dev/nfs boot=nfs nfsroot=${IntIP}:/usr/pluto/diskless/${Moon_DeviceID}/$DefaultBootName,intr,nolock,udp,rsize=32768,wsize=32768,retrans=10,timeo=50 ${BootParams}\n"
			echo -e "$BootConf" > "$Moon_BootConfFile"
		fi

	fi

	# If there are no specific diskless images defined or no DefaultBootName defined in 
	# a referenced diskless image then define a normal MD pxeconfig
	if [[ -n "$Moon_DisklessImages" ]] || [[ -z "$DefaultBootName" ]]; then
		mkdir -p /tftpboot/${Moon_DeviceID}

		## Find kernel and initrd
		Kver=$(basename $(ls ${Moon_RootLocation}/boot/vmlinuz-* | head -1) | cut -d"-" -f2-99)
		Kernel=/usr/pluto/diskless/"$Moon_DeviceID"/boot/vmlinuz-"$Kver"
		Initrd=/usr/pluto/diskless/"$Moon_DeviceID"/boot/initrd.img-"$Kver"

		if [[ -z "$Kernel" || ! -f "$Kernel" || -z "$Initrd" || ! -f "$Initrd" ]]; then
			echo "WARNING: Missing kernel or initrd file. Cannot set for PXE boot."
			continue
		fi

		# Point to the always updated ubuntu softlink at device root.
		if [[ -f ${Moon_RootLocation}/vmlinuz ]]; then
	       		ln -sf ${Moon_RootLocation}/vmlinuz /tftpboot/${Moon_DeviceID}/vmlinuz
		else
			# /vmlinuz was not found, setup the symlinks
			pushd ${Moon_RootLocation}
			ln -sf boot/vmlinuz-${Kver} ${Moon_RootLocation}/vmlinuz
			popd
	       		ln -sf ${Moon_RootLocation}/vmlinuz /tftpboot/${Moon_DeviceID}/vmlinuz
		fi

		if [[ -f ${Moon_RootLocation}/initrd.img ]]; then
			ln -sf ${Moon_RootLocation}/initrd.img /tftpboot/${Moon_DeviceID}/initrd.img
		else
			# /initrd.img was not found, setup the symlink
			pushd ${Moon_RootLocation}
			ln -sf boot/initrd.img-${Kver} ${Moon_RootLocation}/initrd.img
			popd
	       		ln -sf ${Moon_RootLocation}/initrd.img /tftpboot/${Moon_DeviceID}/initrd.img
		fi
		chmod +r /tftpboot/${Moon_DeviceID}/vmlinuz
		chmod +r /tftpboot/${Moon_DeviceID}/initrd.img

		case $Moon_DistroID in
			$DD_DISTRO_Raspbian_Wheezy|$DD_DISTRO_Raspbian_Jessie)
				nfsroot="${IntIP}:/usr/pluto/diskless/${Moon_DeviceID}"
				initrd=""
				;;
			*)
				## these are needed because by default they see the kernel running on the core,
				## which may be different from the one installed on the MD, thus not doing what we want by default
				chroot /usr/pluto/diskless/"$Moon_DeviceID" depmod "$Kver"
				chroot /usr/pluto/diskless/"$Moon_DeviceID" update-initramfs -k "$Kver" -u

				nfsroot="${IntIP}:/usr/pluto/diskless/${Moon_DeviceID},intr,nolock,udp,rsize=32768,wsize=32768,retrans=10,timeo=50"
				initrd="initrd=${Moon_DeviceID}/initrd.img ramdisk=10240 rw"
				;;
		esac

		BootConf="${BootConf}DEFAULT LinuxMCE\n"
		BootConf="${BootConf}LABEL LinuxMCE\n"
		BootConf="${BootConf}KERNEL ${Moon_DeviceID}/vmlinuz\n"
		BootConf="${BootConf}APPEND ${initrd} root=/dev/nfs boot=nfs nfsroot=${nfsroot} ${BootParams}\n"
		echo -e "$BootConf" > "$Moon_BootConfFile"
	fi
}

function setup_mysql_access 
{
	echo "* Setting up MySQL access for MD #${Moon_DeviceID}"
	RunSQL "GRANT ALL PRIVILEGES ON *.* TO '$MySqlUser'@$Moon_IP; GRANT ALL PRIVILEGES ON *.* TO 'eib'@$Moon_IP"
	RunSQL "FLUSH PRIVILEGES"
}


function generate_diskless_installer 
{
	
	echo "* Generating LinuxMCE Installer for MD #${Moon_DeviceID}"

	## Copy installer files
	mkdir -p $Moon_RootLocation/usr/pluto/install
	Files="Common.sh ConfirmDependencies_Debian.sh AptSources.sh"
	for Stuff in $Files; do
		cp /usr/pluto/install/$Stuff $Moon_RootLocation/usr/pluto/install
	done

	Moon_DistroID=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_PK_Distro'")

	## Generate another installer script
	if [[ ! -f $Moon_RootLocation/usr/pluto/install/activation.sh ]]; then
		/usr/pluto/bin/ConfirmDependencies -o "$Moon_DistroID" -r -D "$MySqlDBName" $PLUTO_DB_CRED -d "$Moon_DeviceID" install > $Moon_RootLocation/usr/pluto/install/activation.sh
	fi
}


function setup_hosts_file 
{
	echo "* Setting up /etc/hosts"
	local Content=""
	local Q="
		SELECT 
			PK_Device, 
			IPaddress
		FROM 
			Device
			JOIN Device_DeviceData ON PK_Device = FK_Device AND FK_DeviceData = $DEVICEDATA_DisklessBoot
			JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		WHERE 
			FK_DeviceCategory = '8'
			AND
			FK_Device_ControlledVia IS NULL
			AND
			IK_DeviceData = '1'
	"
	local R=$(RunSQL "$Q")
	local Row
	for Row in $R ;do
		local DeviceID=$(Field 1 "$Row")
		local IP=$(Field 2 "$Row")

		if [[ "$IP" == "" ]] ;then
			continue
		fi

		Content="${Content}${IP}		moon${DeviceID}\n"
	done
	
	PopulateSection "/etc/hosts" "DisklessMD" "$Content"

	## Export hosts file to other computer
	echo | /usr/lib/yp/ypinit -m

	echo "---$Content---"
}


function update_config_files
{
	local ScriptDir="/usr/pluto/bin/files.d"
	local ScriptsList="cron.d-synctime fstab-diskless interfaces mythtv-mysql.txt nis-client pluto.conf resolv.conf syslog.conf timezone net-fix.conf mountnfs.sh hostname mythtv-fix firstrun"
	local -a Names
	local NameKernel Name Kernel
	if [[ -n "$Moon_DisklessImages" ]]; then
		for line in $Moon_DisklessImages; do
			NameKernel="${line%%=*}"
			if [[ "$NameKernel" == *-* ]]; then
				Name="${NameKernel%%-*}"
				Kernel="${NameKernel#*-}"
			else
				Name="$NameKernel"
				Kernel=
			fi
			[[ -z "$Kernel" ]] && continue || :
			Names=("${Names[@]}" "$Name")
		done
	fi
	pushd "$ScriptDir" >/dev/null
	for Script in $ScriptsList ;do
		echo "INFO : Executing script $Script"
		if [[ ! -x $ScriptDir/$Script ]] ;then
			echo "WARNING: Script $Script cannot be executed"
			continue
		fi

		if [[ "${#Names[@]}" -eq 0 ]]; then
			$ScriptDir/$Script --root "$Moon_RootLocation" --device "$Moon_DeviceID" --template "$Moon_DeviceTemplate"
		else
			for Name in "${Names[@]}"; do
				$ScriptDir/$Script --root "$Moon_RootLocation/$Name" --device "$Moon_DeviceID" --subname "$Name" --template "$Moon_DeviceTemplate"
			done
		fi
	done
	popd >/dev/null
}


## Parse parameters
SkipLock=n
for ((i = 1; i <= "$#"; i++)); do
        Parm="${!i}"
        case "$Parm" in                 --skiplock) SkipLock=y
        esac
done


## Aquire Lock
if [[ "$SkipLock" == n ]]; then
        if ! TryLock "Diskless_Setup" "Diskless_Setup"; then
                echo "Another Diskless-related operation is running"
                exit 1
        fi
        trap 'Unlock "Diskless_Setup" "Diskless_Setup"' EXIT
fi


## Check for valid DHCP settings else exit
if [ -z "$DHCPsetting" ]; then
	echo "Diskless MDs can't exist in the current setup (no DHCP). Not setting up any."
	exit
fi


## Getting a list of Media Directors
echo "Getting list of Media Directors"
Q="
	SELECT 
		PK_Device, 
		IPaddress, 
		MACaddress, 
		Device.Description,
		Device.NeedConfigure,
		Device.FK_DeviceTemplate
	FROM 
		Device
		JOIN Device_DeviceData ON PK_Device = FK_Device AND FK_DeviceData = $DEVICEDATA_DisklessBoot
		JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
	WHERE 
		FK_DeviceCategory = '8'
		AND
		FK_Device_ControlledVia IS NULL
		AND
		IK_DeviceData = '1'
"
R=$(RunSQL "$Q")

## Processing Moons
for Row in $R; do
	## Gathering information about this moon
	Moon_DeviceID=$(Field 1 "$Row")

	Moon_IP=$(Field 2 "$Row")
	Moon_IP=$(/usr/pluto/bin/PlutoDHCP.sh -d "$Moon_DeviceID" -a)
	if [[ -z "$Moon_IP" ]]; then
		echo "WARNING : No free IP left to assign for moon${Moon_DeviceID}"
		continue
	fi
	
	Moon_MAC=$(Field 3 "$Row")
	Moon_MAC=$(echo ${Moon_MAC//-/:} | tr 'a-z' 'A-Z')
	if ! /usr/pluto/bin/CheckMAC.sh "$Moon_MAC" ;then
		echo "WARNING : Mac ($Moon_MAC) of moon${Moon_DeviceID} is invalid"
		continue
	fi

	Moon_Description=$(Field 4 "$Row")
	Moon_NeedConfigure=$(Field 5 "$Row")
	if [[ "$Moon_NeedConfigure" != "1" ]] ;then
		echo "INFO : Skiping moon${Moon_DeviceID} because NeedConfigure flag is not set"
		continue
	fi
	echo "INFO : Processing moon${Moon_DeviceID} because NeedConfigure flag is set"

	Moon_DeviceTemplate=$(Field 6 "$Row")

	Moon_Architecture=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_Architecture'")
	if [[ "$Moon_Architecture" == "" ]] ;then
		Moon_Architecture=i386
	fi

	Moon_Model=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_Model'")
	Moon_Model="${Moon_Model%_*}_${Moon_Architecture}"
	RunSQL "UPDATE Device_DeviceData SET IK_DeviceData='$Moon_Model' WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_Model'"

	Moon_RootLocation="/usr/pluto/diskless/${Moon_DeviceID}"
	Moon_BootConfFile="/tftpboot/pxelinux.cfg/01-$(echo ${Moon_MAC//:/-} | tr 'A-Z' 'a-z')"
	Moon_DisklessImages=$(GetDeviceData "$Moon_DeviceID" "$DEVICEDATA_DisklessImages")

	## Adding moon to hosts (/etc/hosts)
	hosts_DisklessMD="${hosts_DisklessMD}${Moon_IP}	moon${Moon_DeviceID}\n"

	## Create a filesystem for this MD
	echo "INFO : Run Diskless_CreateFS.sh $Moon_DeviceID"
	/usr/pluto/bin/Diskless_CreateFS.sh "$Moon_DeviceID"
	echo "INFO : Run Diskless_InstallKernel.sh $Moon_DeviceID"
	/usr/pluto/bin/Diskless_InstallKernel.sh "$Moon_DeviceID"

	## Setting Up
	echo "INFO : Updating config files for Moon $Moon_DeviceID"
	update_config_files
	echo "INFO : Setting up TFTP boot for Moon $Moon_DeviceID"
	setup_tftp_boot
	echo "INFO : Setting up mysql access for Moon $Moon_DeviceID"
	setup_mysql_access
	echo "INFO : Generating diskless installer for Moon $Moon_DeviceID"
	generate_diskless_installer

#	## Create /var/log/pluto for this device as a symlink
#	mkdir -p "/home/logs/diskless_${Moon_DeviceID}"
#	if [[ -d $Moon_RootLocation/var/log/pluto	]] ;then
#		mv $Moon_RootLocation/var/log/pluto/* /home/log/diskless_$Moon_DeviceID
#		rm -rf $Moon_RootLocation/var/log/pluto/
#	fi
#	if [[ ! -e "$Moon_RootLocation"/var/log/pluto ]]; then
#		ln -s "/home/logs/diskless_$Moon_DeviceID" "$Moon_RootLocation/var/log/pluto"
#	fi

	## Configure ssh for this device
	if [[ ! -d ${Moon_RootLocation}/root/.ssh ]]; then 
		mkdir -p ${Moon_RootLocation}/root/.ssh
	fi
	cat /usr/pluto/keys/id_dsa_pluto.pub > ${Moon_RootLocation}/root/.ssh/authorized_keys

	## Done configuring this MD
	# Do not reset this here or Config_Device_Changes.sh will not run to install pkgs for devices in the dev tree
	#RunSQL "UPDATE Device SET NeedConfigure = 0 WHERE PK_Device=$Moon_DeviceID"

	if [[ -n "$(find ${Moon_RootLocation}/var/cache/apt/archives/ -iname '*.deb')" ]]; then
		TARGET_DISTRO=$(LC_ALL="C" chroot ${Moon_RootLocation} lsb_release -i -s | tr '[:upper:]' '[:lower:]')
		TARGET_RELEASE=$(LC_ALL="C" chroot ${Moon_RootLocation} lsb_release -c -s)
		TARGET_ARCH=$(LC_ALL="C" chroot ${Moon_RootLocation} apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
		DEB_CACHE=$TARGET_DISTRO-$TARGET_RELEASE-$TARGET_ARCH

		echo "Moving this MDs apt cache to /usr/pluto/deb-cache/$DEB_CACHE"
		find ${Moon_RootLocation}/var/cache/apt/archives/ -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/$DEB_CACHE \;
		/usr/pluto/bin/update-debcache.sh /usr/pluto/deb-cache/$DEB_CACHE
	fi
done

echo "Setting up hosts file"
setup_hosts_file

#echo "Updating startup scripts"
/usr/pluto/bin/Update_StartupScrips.sh

echo "Exporting diskless filesystems"
/usr/pluto/bin/Diskless_ExportsNFS.sh

#Only run this script if the Asterisk database is there
echo "Synching pluto 2 asterisk"
if $(mysql --batch --skip-column-names -e "SHOW DATABASES LIKE 'asterisk'" | grep asterisk); then
	/usr/pluto/bin/sync_pluto2amp.pl
fi

## Add host to the list of hosts allowed to access /home samba share
echo "Add host to the list of hosts allowed to access /home share"
R=$(RunSQL "SELECT IPaddress FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate WHERE FK_DeviceCategory = 8")
MoonIPs=""
for Row in $R; do
	IPaddress=$(Field 1 "$Row")
	if [[ "$IPaddress" == "" ]] ;then
		continue;
	fi

	MoonIPs="${MoonIPs} $IPaddress"
done

if [[ $(CheckSectionExists "/etc/samba/smb.conf" "Home Hosts Allow") == "true" ]] ;then
	PopulateSection "/etc/samba/smb.conf" "Home Hosts Allow" "hosts allow = $MoonIPs"
fi

echo "Checking number of users, auto reload if less than 1"
. /usr/pluto/bin/Config_Ops.sh
if [[ "$PK_Users" -lt "1" ]]; then
	/usr/pluto/bin/MessageSend "dcerouter" 0 7 7 1 163 "First MD headless reload"
fi
echo "Finished setting up network boot for media directors."
echo "If new media director(s) were added, do a quick reload router."
