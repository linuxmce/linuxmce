#!/bin/bash

Type="$1"
. /usr/pluto/install/Common.sh
. /usr/pluto/bin/Utils.sh

if [ "$Type" == "diskless" -a -f /etc/diskless.conf ]; then
	. /etc/diskless.conf
fi

echo "** Initial config script"

PHURL="http://www.plutohome.com/"
ACTIV="http://activate.plutohome.com"

activation_url="$ACTIV/activation.php"
activation_url_alt="$ACTIV/pluto-admin/activation.php"

DIR="/usr/pluto/install"
CDD="ConfirmDependencies_Debian.sh"
ICS="Initial_Config.sh"
wget -O "$DIR"/"$CDD"_new "$ACTIV/confirm_dependencies_debian.php" 2>/dev/null
[ "$?" -eq 0 ] && mv "$DIR"/"$CDD"_new "$DIR"/"$CDD"
wget -O "$DIR"/"$ICS"_new "$ACTIV/debian_install_script.php" 2>/dev/null
[ "$?" -eq 0 ] && mv "$DIR"/"$ICS"_new "$DIR"/"$ICS"
rm -f "$DIR"/*_new

ICS_MSG="When you have internet connection, run '$DIR/$ICS' to download your activation script from plutohome.com"
NOCODE_MSG="Whenever you get a code from $PHURL, you can run '$DIR/$ICS' to get back to this script"
NOACTIV_MSG="There was a problem while downloading the activation script. To reattempt an installation, you can run anytime '$DIR/$ICS' without using the installation CD"
ACTIV_MSG="There was a problem while retrieving/processing the activation data. You can re-run the script anytime later to try again by typing '$DIR/activation.sh' at your shells prompt"
#BUILD_MSG="We created a file called '$DIR/build.sh' that you can run to make any required packages from source"
#BUILD_ALL_MSG="We created a file called '$DIR/build_all.sh' which will make the entire Pluto system from source"

# ask "Try again?" question; return 1 on "Nn", 0 otherwise
try_again()
{
	ClearBlue
	echo "$(CatMessages "$@")" | fmt
	echo -n "Do you want to try again? (Y/n): "
	read again
#	again=$(QuestionBox "$@" "Do you want to try again?")
	[ "$again" == "N" -o "$again" == "n" ] && return 1
	ClearBlack
	return 0
}

RestoreCoreConf()
{
	[ -d /home/backup ] || return 0
	[ -f /home/backup/pluto.conf-Core ] || return 0

	sed "$NoSpace" /home/backup/pluto.conf-Core | egrep -v "^#|^//" >/etc/pluto.conf
	while read line; do
		eval "local $line" &>/dev/null
	done </etc/pluto.conf
	export Device="$PK_Device"
	export Code="$Activation_Code"
}

if [ "$Type" == "router" ]; then
	# /etc/polipo/config
	polipo_conf='# Pluto config for polipo
proxyAddress = "0.0.0.0"
proxyPort = 8123
allowedClients = 127.0.0.1
logFile = /var/log/polipo.log
relaxTransparency = maybe
dnsUseGethostbyname = true
'
	# /etc/frox.conf
	frox_conf='# Pluto config for frox
Port 8124
User frox
Group frox
WorkingDir /var/cache/frox
DontChroot yes
LogLevel 20
LogFile /var/log/frox.log
PidFile /var/cache/frox/frox.pid
BounceDefend yes
CacheModule local
CacheSize 500
CacheAll no
DoNTP yes
MaxForks 50
MaxForksPerHost 4
ACL Allow * - *
'
	# /etc/apt/apt.conf.d/30pluto
	pluto_apt_conf='// Pluto apt conf add-on
Acquire::http::Proxy "http://127.0.0.1:8123";
Acquire::ftp::Proxy "ftp://127.0.0.1:8124";
Acquire::ftp::ProxyLogin { "USER $(SITE_USER)@$(SITE):$(SITE_PORT)"; "PASS $(SITE_PASS)"; };
Acquire::http::Proxy::dcerouter "DIRECT";
Apt::Cache-Limit "12582912";
'

	if dpkg -i /usr/pluto/install/deb-critical/*; then
		echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto
		echo -n "$polipo_conf" >/etc/polipo/config
		echo -n "$frox_conf" >/etc/frox.conf
		/usr/sbin/adduser --system --group --home /var/cache/frox --disabled-login --disabled-password frox
		echo "RUN_DAEMON=yes" >/etc/default/frox
		/etc/init.d/frox restart
		/etc/init.d/polipo restart
	else
		echo "* ERROR * Complete proxy combination is not available. Installation can't continue"
		read
		exit 1
	fi
	RestoreCoreConf
	WhereCode="in pluto.conf backup"
else
	WhereCode="in diskless.conf"
fi

if ! BlacklistConfFiles '/etc/apt/sources.list' ;then
	if [ ! -e /etc/apt/sources.list.pbackup ] ;then
		cp /etc/apt/sources.list  /etc/apt/sources.list.pbackup
	fi
	echo "deb file:/usr/pluto/deb-cache/ sarge main" | cat - /etc/apt/sources.list >/etc/apt/sources.list.2
	mv /etc/apt/sources.list.2 /etc/apt/sources.list
	apt-get update &>/dev/null
fi

# Format: OK-Device-Code
ok=0
mac=$(/sbin/ifconfig eth0|grep HWaddr|awk '{ print $5; }'|sed s/://g)
while [ "$ok" -eq 0 ]; do
	echo "Getting pre-activation data (if any)"
	answer=$(wget -O - "$activation_url?mac=$mac" 2>/dev/null)
	RetCode=$?
	if [ "$RetCode" -ne 0 ]; then
		answer=$(wget -O - "$activation_url_alt?mac=$mac" 2>/dev/null)
		RetCode=$?
		[ "$RetCode" -eq 0 ] && activation_url="$activation_url_alt"
	fi
	if [ "$RetCode" -ne 0 ]; then
		try_again "Failed to contact activation server over the Internet" && continue
		echo "$ICS_MSG"
		break
	fi
	result=$(echo $answer |cut -d- -f1)
	if [ "$result" == "OK" ]; then
		Device=$(echo $answer |cut -d- -f2)
		Code=$(echo $answer |cut -d- -f3)
	fi
	ok=1
done

ok=0
while [ "$ok" -eq 0 ]; do
# ask user for activation key
	ok_key=0
	if [ -n "$Device" -a -n "$Code" ]; then
		clear
		echo "Found Device and Activation Code in $WhereCode"
		echo "Device: $Device"
		echo "Activation Code: $Code"
		ok_key=1
		device="$Device"
		code="$Code"
		unset Device Code
	fi

	ok_device="$ok_key"
	while [ "$ok_device" -ne 1 ]; do
		Message1="What device is this going to be?"
		Message2="The device number is listed on step 8 of the new installation wizard at plutohome.com"

		ClearBlue
		echo "$Message1" | fmt
		echo "$Message2" | fmt
		echo -n "Device number: "
		read device
		[ -z "$device" ] && echo "Empty device number" || ok_device=1
		ClearBlack
		
#		device=$(InputBox "$Message1" "$Message2")
#		[ -z "$device" ] && MessageBox "Empty device number" || ok_device=1
	done

	ok_code="$ok_key"
	while [ "$ok_code" -ne 1 ]; do
		Message1="What is your activation code for this installation from plutohome.com?"

		ClearBlue
		echo "$Message1" | fmt
		echo -n "Activation code: "
		read code
		[ -z "$code" ] && echo "Empty activation code" || ok_code=1
		ClearBlack

#		code=$(InputBox "$Message1")
#		[ -z "$code" ] && MessageBox "Empty activation code" || ok_device=1
	done

	activation_key="$device-$code"

# try to get data off the Internet
	ok_internet=0
	while [ "$ok_internet" -eq 0 ]; do
		echo "Getting activation script"
		answer=$(wget -O - "$activation_url?code=$activation_key" 2>/dev/null)
		if [ "$?" -ne 0 ]; then
			try_again "Failed to contact activation server over the Internet" && continue
			echo "$ICS_MSG" | fmt
#			MessageBox "$ICS_MSG"
			exit
		fi
		ok_internet=1
	done

	[ "$ok_internet" -eq 0 ] && break

# verify activation answer
	result=$(echo "$answer" | head -1)
	if [ "$result" != "OK" ]; then
		Message1="Failed getting activation data. Server answer was:"
		Message2="$result"
		try_again "$Message1" "$Message2" && continue
		echo "$NOCODE_MSG" | fmt
#		MessageBox "$NOCODE_MSG"
		exit
	fi

# everything went ok
	echo "Code accepted"

	ok_script=0
	while [ "$ok_script" -eq 0 ]; do
		echo "$answer" | awk 'NR>1' >"$DIR/activation.sh"
		if [ "$(tail -1 "$DIR/activation.sh")" != "#EOF" ]; then
			try_again "Activation script transfer failed" && continue
			echo "$NOACTIV_MSG" | fmt
#			MessageBox "$NOACTIV_MSG"
			exit
		else
			ok_script=1
		fi
	done
	
	Device=$(echo "$activation_key" | cut -d- -f1)
	Code=$(echo "$activation_key" | cut -d- -f2)

	[ "$Type" == "diskless" ] && Host="dcerouter"
	[ "$Type" == "router" ] && Host="localhost"

	pluto_conf="# Pluto config file
MySqlHost = $Host
MySqlUser = root
MySqlPassword = 
MySqlDBName = pluto_main
DCERouter = $Host
MySqlPort = 3306
DCERouterPort = 3450
#<-mkr_b_videowizard_b->
UseVideoWizard = 1
#<-mkr_b_videowizard_e->
TestInstallation = <-mkr_t_test_installation->
PK_Device = $Device
Activation_Code = $Code"
	[ -f /etc/pluto.conf ] || echo "$pluto_conf" >/etc/pluto.conf
	chmod 666 /etc/pluto.conf # that 666 is octal, and equals 438 in decimal :)
	
	ok=1
done

[ "$ok" -eq 0 ] && exit 1

#wget -O "$DIR"/build.sh "$ACTIV/build.php?code=$activation_key" 2>/dev/null || no_build=1
#wget -O "$DIR"/build_all.sh "$ACTIV/build_all.php?code=$activation_key" 2>/dev/null || no_build_all=1

if [ "$Type" == "router" ]; then
	selectedInterface=$(grep 'iface..*eth' /etc/network/interfaces | awk '{print $2}')
	dcerouterIP=$(ifconfig $selectedInterface | awk 'NR==2' | cut -d: -f2 | cut -d' ' -f1)

	hosts="
127.0.0.1       localhost.localdomain   localhost
$dcerouterIP    dcerouter $(/bin/hostname)

# The following lines are desirable for IPv6 capable hosts
::1     ip6-localhost ip6-loopback
fe00::0 ip6-localnet
ff00::0 ip6-mcastprefix
ff02::1 ip6-allnodes
ff02::2 ip6-allrouters
ff02::3 ip6-allhosts
"
	echo "$hosts" >/etc/hosts
fi

clear

chmod +x "$DIR"/activation.sh
if bash -x "$DIR"/activation.sh; then
	echo "Activation went ok"
else
	echo "$ACTIV_MSG" | fmt
fi

if [ "$Type" == "router" ]; then
	if [ -d /home/backup -a -f /home/backup/entire_database.sql ]; then
		echo "Restoring database"
		mysql < /home/backup/entire_database.sql
	fi
fi

#if [ -z "$no_build" -a -s "$DIR"/build.sh ]; then
#	chmod +x "$DIR"/build.sh
#	echo "$BUILD_MSG" | fmt
#fi
#if [ -z "$no_build_all" -a -s "$DIR"/build_all.sh ]; then
#	chmod +x "$DIR"/build_all.sh
#	echo "$BUILD_ALL_MSG" | fmt
#fi

# At this point these scripts should be available (installation finished, didn't it?)
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

Q="SELECT FK_DeviceCategory FROM DeviceTemplate JOIN Device ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device='$device'"
R=$(RunSQL "$Q")

case "$R" in
	# Core
	7)
		SysType="Core"
	;;

	# Media Director
	8)
		SysType="Media Director"
	;;

	# Other (usually, this means it's a bug)
	*)
		SysType="Unknown Device Type '$R'"
	;;
esac

apt-get clean

#wget -O "$DIR/message.txt" "$ACTIV/message.php?code=$CODE" 2>/dev/null && cat "$DIR/message.txt"
Message1="Congratulations.  Pluto installation has completed.
The system will now reboot.  The Pluto Core software will
be started automatically.  As soon as the computer finishes
rebooting you will be able to access the Pluto Admin website
to configure it, and you can start plugging in your media
directors and other plug-and-play devices.  If you are an
advanced Linux user and want to access a terminal before
the reboot, press ALT+F2.  Otherwise..."
Key="the Enter key"
#Key="OK"
Message2="Press $Key to reboot and startup your new Pluto $SysType."

Message24="
[1;5;31m*** WARNING ***[0m You installed the system using the 2.4 kernel.

The system will reboot using a 2.6 kernel, but the two versions see
Serial-ATA ports in different ways. If you only have Paralel-ATA units,
you can reboot without changes. Otherwise, read on.

For now, you have to manually update these files:
/etc/fstab:
	replace /dev/hdX* with /dev/sdT*
		X, T = letters
		ex: /dev/hde -> /dev/sda; /dev/hde1 -> /dev/sda1
/boot/grub/menu.lst:
	For the \"Debian GNU/Linux, kernel 2.6.12-pluto-1-686\" kernel entry you
	need to specify 'root=/dev/sdT1' instead of 'root=/dev/hdX1'

Switch to another console (Alt+F2), login as root and use an editor like
'nano' or 'vi' to edit those files before you reboot"

echo "$(CatMessages "$Message1" "$Message2")" | fmt
Kver=$(uname -r)
if [ "${Kver:0:4}" == "2.4." ]; then
	echo "$Message24"
fi
read
#MessageBox "$Message1" "$Message2" "$Message3"

[ -e /root/.vimrc ] || cp /usr/share/vim/vim63/vimrc_example.vim /root/.vimrc

echo "Setting debconf front-end to Noninteractive"
awk '/Name: debconf\/frontend/,/^$/ {if ($1 == "Value:") print "Value: Noninteractive"; else print; next}
	{print}' /var/cache/debconf/config.dat > /var/cache/debconf/config.dat.$$
mv /var/cache/debconf/config.dat.$$ /var/cache/debconf/config.dat

if [ "$Type" == "diskless" ]; then
	# Replace Initial_Config.sh entry with regular one in inittab
	awk '
		/1:23:once/ { next }
		/^#1:2345:respawn/ { print substr($0, 2); next }
		{ print }
	' /etc/inittab >/etc/inittab.new
	mv -f /etc/inittab.new /etc/inittab
	reboot
fi
#init q
exit 0
