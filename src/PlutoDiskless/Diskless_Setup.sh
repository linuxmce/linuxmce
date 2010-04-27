#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Diskless_Utils.sh
. /usr/pluto/bin/SQL_Ops.sh

. /usr/pluto/bin/TeeMyOutput.sh --outfile "/var/log/pluto/Diskless_Setup.log" --stdboth --append -- "$@"
set -x

SkipLock=n

# Parse parameters
for ((i = 1; i <= "$#"; i++)); do
        Parm="${!i}"
        case "$Parm" in                 --skiplock) SkipLock=y
        esac
done

if [[ "$SkipLock" == n ]]; then
        if ! TryLock "Diskless_Setup" "Diskless_Setup"; then
                echo "Another Diskless-related operation is running"
                exit 1
        fi
        trap 'Unlock "Diskless_Setup" "Diskless_Setup"' EXIT
fi

DEVICEDATA_Extra_Parameters=139
DEVICEDATA_Extra_Parameters_Override=140
DEVICEDATA_Architecture=112

DefaultBootParams="noirqdebug vga=normal"
# vars:
# CORE_INTERNAL_ADDRESS
# INTERNAL_SUBNET
# INTERNAL_SUBNET_MASK
# MOON_ADDRESS
# MOON_IP
# DYNAMIC_IP_RANGE
# KERNEL_VERSION
# MOON_HOSTS
# NOBOOT_ENTRIES
# ENABLE_SPLASH
# MOON_BOOTPARAMS
# MOON_ID

Vars="CORE_INTERNAL_ADDRESS INTERNAL_SUBNET INTERNAL_SUBNET_MASK MOON_ADDRESS DYNAMIC_IP_RANGE KERNEL_VERSION MOON_HOSTS MOON_IP NOBOOT_ENTRIES ENABLE_SPLASH MOON_BOOTPARAMS MOON_ID"


## Set up some variables
KERNEL_VERSION="$(uname -r)"
CORE_INTERNAL_INTERFACE="$IntIf"
CORE_INTERNAL_ADDRESS="$IntIP"
INTERNAL_SUBNET_MASK="$IntNetmask"
for i in 1 2 3 4; do
	IPDigit=$(echo $CORE_INTERNAL_ADDRESS | cut -d. -f$i)
	MaskDigit=$(echo $INTERNAL_SUBNET_MASK | cut -d. -f$i)
	NetDigit=$(($IPDigit & $MaskDigit))
	INTERNAL_SUBNET="$INTERNAL_SUBNET$Dot$NetDigit" && Dot="."
done
if [[ "$(</proc/cmdline)" == *splash* ]]; then
	ENABLE_SPLASH="splash"
fi
MOON_HOSTS=""
DlDir="/usr/pluto/diskless"

## Check for valid DHCP settings else exit
if [ -z "$DHCPsetting" ]; then
	echo "Diskless MDs can't exist in the current setup (no DHCP). Not setting up any."
	exit
fi

## Getting a list of Media Directors
echo "Getting list of Media Directors"
# TODO: filter by installation too (currently only one installation is present, so it works :P)
Q="
	SELECT 
		PK_Device, 
		IPaddress, 
		MACaddress, 
		Device.Description,
		Device.NeedConfigure
	FROM 
		Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
	WHERE 
		PK_DeviceTemplate=28 
		AND 
		FK_Device_ControlledVia IS NULL
"
R=$(RunSQL "$Q")

mkdir -p /tftpboot/pxelinux.cfg

## Processing Moons
for Client in $R; do
	## Read information for the database about this moon
	PK_Device=$(Field 1 "$Client")
	IP=$(Field 2 "$Client")
	MAC=$(Field 3 "$Client")
	Description=$(Field 4 "$Client")
	NeedConfigure=$(Field 5 "$Client")

		
	## Allocating IP for moon
	#FIXME: Is this next step necesary if IP!="" ?
	IP=$(/usr/pluto/bin/PlutoDHCP.sh -d "$PK_Device" -a)
	if [[ -z "$IP" ]]; then
		echo "*** WARNING *** No free IP"
		continue
	fi

	HexIP=$(gethostip -x "$IP")
	lcdMAC=$(echo ${MAC//:/-} | tr 'A-Z' 'a-z')
	MAC=$(echo ${MAC//-/:} | tr 'a-z' 'A-Z')
	MOON_IP="$IP"
	MOON_ID="$PK_Device"
	MOON_ADDRESS="$IP moon$MOON_ID"
	DlPath="$DlDir/$MOON_ID"
	
	## Check if this MD is diskless or not
	Q="
		SELECT 
			PK_Device
		FROM 
			Device_DeviceData
			JOIN Device ON PK_Device=FK_Device
			JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
		WHERE
			FK_DeviceTemplate=28 
			AND 
			FK_DeviceData=9 
			AND 
			IK_DeviceData='1' 
			AND 
			PK_Device='$PK_Device'
	"
	Diskless=$(RunSQL "$Q")

	## If is a diskless md
	if [[ -n "$Diskless" && "$NeedConfigure" == "1" ]]; then
		echo "* Diskless filesystem"

		Q="
			SELECT
				IK_DeviceData
			FROM
				Device_DeviceData
			WHERE
				FK_Device='$PK_Device'
				AND
				FK_DeviceData='$DEVICEDATA_Architecture'
		"
		Architecture=$(RunSQL "$Q")
		if [[ "$Architecture" == "" ]] ;then
			Architecture="686"
		fi
		
		## Create the a filesystem for this MD
		#FIXME: Should we create this filesystem everytime ?
		/usr/pluto/bin/Diskless_CreateFS.sh "$IP" "$MAC" "$PK_Device" "$Architecture"
		
		## Check to see if we have a valid hardware adreess
		/usr/pluto/bin/CheckMAC.sh "$MAC"
		ValidMAC=$?

		## If is a valid MAC address then set up pxelinux for this machine
		if [[ "$ValidMAC" -eq 0 ]]; then
			echo "* Setting up /tftpboot/pxelinux.cfg/01-$lcdMAC"
			
			OverrideDefaults=0
			ExtraBootParams=			
			Q="
				SELECT 
					FK_DeviceData,
					IK_DeviceData
				FROM
					Device_DeviceData
				WHERE 
					FK_Device='$PK_Device' 
					AND
					FK_DeviceData IN ($DEVICEDATA_Extra_Parameters, $DEVICEDATA_Extra_Parameters_Override)
			"
			ExtraParms=$(RunSQL "$Q")
			
			for ExtraParm in $ExtraParms; do
				FK_DeviceData=$(Field 1 "$ExtraParm")
				IK_DeviceData=$(Field 2 "$ExtraParm")
				case "$FK_DeviceData" in
					"$DEVICEDATA_Extra_Parameters")
						ExtraBootParams="$IK_DeviceData"
						;;
					"$DEVICEDATA_Extra_Parameters_Override")
						OverrideDefaults="$IK_DeviceData"
						;;
				esac
			done

			if [[ "$OverrideDefaults" == 1 ]]; then
				MOON_BOOTPARAMS="$ExtraBootParams"
			else
				MOON_BOOTPARAMS="$DefaultBootParams $ExtraBootParams"
			fi

			KERNEL_VERSION=$(DecideKernelVersion "$Architecture" | tail -1)

			cp /usr/pluto/templates/pxelinux.tmpl /tftpboot/pxelinux.cfg/01-$lcdMAC.$$
			ReplaceVars /tftpboot/pxelinux.cfg/01-$lcdMAC.$$
			mv /tftpboot/pxelinux.cfg/01-$lcdMAC.$$ /tftpboot/pxelinux.cfg/01-$lcdMAC

		## If is not a valid MAC address the we should inform the user about this
		else
			echo "* MAC address is wrong. You won't be able to boot this until you correct it."
		fi

		
		echo "* Setting local files"

		## Setup pluto.conf
		echo -n " pluto.conf"
		if [[ ! -f $DlPath/etc/pluto.conf ]] ;then
			if [[ "$OfflineMode" == "" ]]; then
				OfflineMode='false'
			fi

			pluto_conf="OfflineMode = $OfflineMode \n"
			pluto_conf="${pluto_conf}PlutoVersion = $PlutoVersion \n"
			pluto_conf="${pluto_conf}Vendor = $Vendor \n"
			pluto_conf="${pluto_conf}Display = $Display \n"
			pluto_conf="${pluto_conf}LogLevels = 1,5,7,8 \n"
			pluto_conf="${pluto_conf}#ImmediatelyFlushLog = 1 \n"
			echo -e $pluto_conf > $DlPath/etc/pluto.conf
		fi
		
		## Setup nis
		echo -n " nis"
		cp /usr/pluto/templates/nis-client.template $DlPath/etc/default/nis
		PopulateSection "$DlPath/etc/default/nis" "Nis Master" "NISMASTER=$IntIP"	

		## Setup fstab
		echo -n " fstab"
		cp /usr/pluto/templates/fstab.tmpl $DlPath/etc/fstab.$$
		ReplaceVars $DlPath/etc/fstab.$$
		mv $DlPath/etc/fstab.$$ $DlPath/etc/fstab

		## Setup hosts
		echo -n " hosts"
		cp /usr/pluto/templates/hosts.tmpl $DlPath/etc/hosts.$$
		ReplaceVars $DlPath/etc/hosts.$$
		mv $DlPath/etc/hosts.$$ $DlPath/etc/hosts

		## Setup Interfaces
		echo -n " interfaces"
		cp /usr/pluto/templates/interfaces.diskless.tmpl $DlPath/etc/network/interfaces.$$
		ReplaceVars $DlPath/etc/network/interfaces.$$
		mv $DlPath/etc/network/interfaces{.$$,}

		## Setup resolv.conf
		echo -n " DNS"
		echo "nameserver $CORE_INTERNAL_ADDRESS" >$DlPath/etc/resolv.conf

		## Setup hostname
		echo -n " hostname"
		echo "moon$MOON_ID" >$DlPath/etc/hostname

		## Setup MythTC MySQL settings
		echo -n " MythTV_DB_Settings"
		mkdir -p $DlPath/etc/mythtv
		cp /etc/mythtv/mysql.txt $DlPath/etc/mythtv/mysql.txt.$$
		SedCmd="s/^DBHostName.*/DBHostName=dcerouter/g"
		sed -i "$SedCmd" $DlPath/etc/mythtv/mysql.txt.$$
		mv $DlPath/etc/mythtv/mysql.txt.$$ $DlPath/etc/mythtv/mysql.txt

		## Setup mysql access
		echo -n " MySQL_access"
		Q="GRANT ALL PRIVILEGES ON *.* TO 'root'@$IP;
		GRANT ALL PRIVILEGES ON *.* TO 'eib'@$IP"
		echo "$Q" | /usr/bin/mysql

		## Copy installer files
		echo -n " Install"
		mkdir -p $DlPath/usr/pluto/install
		Files="Common.sh AptSources.sh ConfirmDependencies_Debian.sh Initial_Config_MD.sh Initial_Config_Finish.sh ramdisk.tar.bz2"
		for Stuff in $Files; do
			cp /usr/pluto/install/$Stuff $DlPath/usr/pluto/install
		done

		## Generate another installer script
		echo -n "activation.sh"
		if [[ ! -f $DlPath/usr/pluto/install/activation.sh ]]; then
			/usr/pluto/bin/ConfirmDependencies -r -D pluto_main -h dcerouter -u root -p '' -d $PK_Device install > $DlPath/usr/pluto/install/activation.sh
		fi
		
		## Modify a install script to run as for diskless
		sed '/^Type=/ s/^.*$/Type="diskless"/' /usr/pluto/install/Initial_Config.sh >$DlPath/usr/pluto/install/Initial_Config.sh
		chmod +x $DlPath/usr/pluto/install/Initial_Config.sh
		mkdir -p $DlPath/usr/pluto/deb-cache

		echo -n " Timezone"
		if [ -f /etc/timezone ]; then
			TimeZone="$(cat /etc/timezone)"
		else
			TimeZone="$(readlink /etc/localtime)"
			TimeZone="${TimeZone#/usr/share/zoneinfo/}"
		fi
		echo "$TimeZone" >/etc/timezone
		[ -f "$DlPath/etc/localtime" ] && rm -f $DlPath/etc/localtime # in case it's a file from the old (wrong) way of setting this up
		ln -sf /usr/share/zoneinfo/$TimeZone $DlPath/etc/localtime

		## Configure syslog to log on dcerouter
		echo -n " syslog"
		SysLogCfg1="*.*;auth,authpriv.none		/dev/tty12"
		SysLogCfg2="*.*;auth,authpriv.none		@dcerouter"
		if ! grep -qF "$SysLogCfg1" $DlPath/etc/syslog.conf; then
			echo "$SysLogCfg1" >>$DlPath/etc/syslog.conf
		fi
		if ! grep -qF "$SysLogCfg2" $DlPath/etc/syslog.conf; then
			echo "$SysLogCfg2" >>$DlPath/etc/syslog.conf
		fi

		## Default domain for NIS
		echo -n " nis"
		echo "pluto" >$DlPath/etc/defaultdomain

		echo 

		## Setup cronjob daily for ntpdate
		
		SyncTime="/etc/cron.daily/synctime"

		if [ ! -d $DlPath/etc/cron.daily ]; then
				mkdir -p $DlPath/etc/cron.daily
				Q="SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7"
		                RouterIP=$(RunSQL "$Q")
                		if [[ "$RouterIP" == "" ]] ;then
                         		RouterIP="192.168.80.1"
                		fi

				echo "/usr/sbin/ntpdate $RouterIP" > $DlPath/$SyncTime
				chmod +x $DlPath/$SyncTime
		fi
		
		## Create /var/log/pluto for this device as a symlink
		mkdir -p "/home/logs/diskless_$MOON_ID"
		if [[ -d $DlPath/var/log/pluto	]] ;then
			mv -r $DlPath/var/log/pluto/* /home/log/diskless_$MOON_ID
			rm -rf $DlPath/var/log/pluto/
		fi
		if [[ ! -e "$DlPath"/var/log/pluto ]]; then
			ln -s "/home/logs/diskless_$MOON_ID" "$DlPath/var/log/pluto"
		fi
	
		## Dome configuring this MD
		Q="UPDATE Device SET NeedConfigure = 0 WHERE PK_Device=$PK_Device"
		RunSQL "$Q"
	fi

	echo "* Adding to hosts"
	MOON_HOSTS="$MOON_HOSTS\n$IP\tmoon$MOON_ID"

done

DYNAMIC_IP_RANGE=
if [ "${DHCPsetting%,*}" != "$DHCPsetting" ]; then
	DHCPRange="${DHCPsetting#*,}"
	Range1="${DHCPRange%-*}"
	Range2="${DHCPRange#*-}"

	DYNAMIC_IP_RANGE=$(printf "%s" "\n\tpool {\n\t\tallow unknown-clients;\n\t\trange $Range1 $Range2;\n\t}")
fi

# reset /etc/hosts
# duplicated from Network_Setup.sh
hosts="
127.0.0.1       localhost.localdomain   localhost
$CORE_INTERNAL_ADDRESS	dcerouter $(/bin/hostname)
#%MOON_HOSTS%

# The following lines are desirable for IPv6 capable hosts
::1     ip6-localhost ip6-loopback
fe00::0 ip6-localnet
ff00::0 ip6-mcastprefix
ff02::1 ip6-allnodes
ff02::2 ip6-allrouters
ff02::3 ip6-allhosts
"
echo "$hosts" >/etc/hosts

ReplaceVars /etc/hosts

Q="FLUSH PRIVILEGES"
RunSQL "$Q"

/usr/pluto/bin/Update_StartupScrips.sh

## Do the update fix for current debian computers
for dir in /usr/pluto/diskless/* ;do
	if [[ -f \"\$dir/debian/etc/init.d/fastboot/rcS\" ]] ;then
		sed -i 's/ApplyUpdates.sh/LMCEUpdate_Apply.sh/g' \"\$dir/debian/etc/init.d/fastboot/rcS\" || :
		rm -f \"\$dir/debian/etc/rc2.d/S98LMCEUpdate\" || :
	fi
done


echo "Finished setting up network boot for media directors."
echo "If new media director(s) were added, do a quick reload router."
