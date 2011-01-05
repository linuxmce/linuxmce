#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

## Misc variables
DISTRO="$(lsb_release -c -s)"
DEVICEDATA_Architecture=112
Architecture=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g')

if [ -z "$KVER" ]
then
	KVER=$(uname -r)
fi

DBST_SCRIPT='/usr/pluto/bin/Diskless_DebootstrapPluto.sh'
ARH_DIR='/usr/pluto/install'
DisklessFS="PlutoMD-${Architecture}.tar.bz2"

Installer_Files=(BonusCdMenu.sh BonusCdAutoInst.sh Common.sh AptSources.sh ConfirmDependencies_Debian.sh Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh Initial_DHCP_Config.sh Initial_Network_Config.sh pkgsel post-debootstrap.sh preseed.cfg PreseedStage2.sh)

## Create a temporary directory to store the diskless image into and be sure
## it gets removed after we finish
TEMP_DIR=$(mktemp -d)
#TEMP_DIR="/var/tmp/DisklessTBZ"; mkdir -pv $TEMP_DIR

function trap_EXIT() {
	umount -fl $TEMP_DIR/var/cache/apt
	umount -fl $TEMP_DIR/usr/pluto/deb-cache
	umount -fl $TEMP_DIR/proc
	umount -fl $TEMP_DIR/sys
	umount -fl $TEMP_DIR/lib/modules/${KVER}/volatile

#	rm -rf $TEMP_DIR
}

trap "trap_EXIT" EXIT


function create_initial_root_with_debootstrap {
	debootstrap "$(lsb_release -c -s)" "$TEMP_DIR" "http://archive.ubuntu.com/ubuntu/"
}

function create_initial_root_with_archive {
	pushd $TEMP_DIR
		echo "untarring $ARH_DIR/PlutoMD_Debootstraped.tar.bz2"
		tar -xf "$ARH_DIR"/PlutoMD_Debootstraped.tar.bz2
	popd
}

if [[ -r /usr/pluto/install/PlutoMD_Debootstraped.tar.bz2 ]] ;then
	create_initial_root_with_archive
else
	create_initial_root_with_debootstrap
fi

# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
echo "[client]
host = dcerouter
" > "$TEMP_DIR"/root/.my.cnf



## Setup debconf interface to 'noninteractive'
export DEBIAN_FRONTEND=noninteractive
echo "Setting Debconf interface to 'noninteractive' and default priority 'critical'"
awk '
/^Name: debconf\/frontend$/,/^$/ {
	if ($1 == "Value:")
		print "Value: noninteractive";
	else
		print;
	next;
}
/^Name: debconf\/priority$/,/^$/ {
	if ($1 == "Value:")
		print "Value: critical";
	else
		print;
	next;
}
{print}
' "$TEMP_DIR"/var/cache/debconf/config.dat >"$TEMP_DIR"/var/cache/debconf/config.dat.$$
mv "$TEMP_DIR"/var/cache/debconf/config.dat{.$$,}

if ! grep -q '^Name: shared/accepted-sun-dlj-v1-1' "$TEMP_DIR"/var/cache/debconf/config.dat ;then
	echo 'Name: shared/accepted-sun-dlj-v1-1'     >> "$TEMP_DIR"/var/cache/debconf/config.dat
	echo 'Template: shared/accepted-sun-dlj-v1-1' >> "$TEMP_DIR"/var/cache/debconf/config.dat
	echo 'Value: true'                            >> "$TEMP_DIR"/var/cache/debconf/config.dat
	echo 'Owners: sun-java6-bin, sun-java6-jre'   >> "$TEMP_DIR"/var/cache/debconf/config.dat
	echo 'Flags: seen'                            >> "$TEMP_DIR"/var/cache/debconf/config.dat
fi


## Set up chroot installation environment
## FIXME: Do we need this ?
touch "$TEMP_DIR"/etc/chroot-install

## Disable invoke-rc.d scripts
mv "$TEMP_DIR"/usr/sbin/invoke-rc.d{,.pluto-install}
mv "$TEMP_DIR"/sbin/start-stop-daemon{,.pluto-install}
echo -en '#!/bin/bash\necho "WARNING: fake invoke-rc.d called"\n' >"$TEMP_DIR"/usr/sbin/invoke-rc.d
echo -en '#!/bin/bash\necho "WARNING: fake start-stop-daemon called"\n' >"$TEMP_DIR"/sbin/start-stop-daemon
chmod +x "$TEMP_DIR"/usr/sbin/invoke-rc.d
chmod +x "$TEMP_DIR"/sbin/start-stop-daemon

## Setup initial resolv.conf
cp /etc/resolv.conf "$TEMP_DIR"/etc/resolv.conf

## Create the temporary sources.list file for the media director 

## Enable some needed mount points
mkdir -p $TEMP_DIR/var/cache/apt
mkdir -p $TEMP_DIR/usr/pluto/deb-cache
mount --bind /var/cache/apt $TEMP_DIR/var/cache/apt
mount --bind /usr/pluto/deb-cache $TEMP_DIR/usr/pluto/deb-cache
mount none -t sysfs $TEMP_DIR/sys
mount none -t proc  $TEMP_DIR/proc

## Setup apt in pluto style
## FIXME: maybe we need to make sources.list from scratch ?
cp {,"$TEMP_DIR"}/etc/apt/sources.list
[[ -f /etc/apt/apt.conf.d/30pluto ]] && cp {,"$TEMP_DIR"}/etc/apt/apt.conf.d/30pluto
[[ -f /etc/apt/preferences ]] && cp {,"$TEMP_DIR"}/etc/apt/preferences
[[ -f /etc/apt/apt.conf ]] && cp {,"$TEMP_DIR"}/etc/apt/apt.conf



## Update the chrooted system (needed when created from archive)
#chroot $TEMP_DIR /bin/bash
LC_ALL=C chroot $TEMP_DIR apt-get -y update
LC_ALL=C chroot $TEMP_DIR apt-get -f -y install
LC_ALL=C chroot $TEMP_DIR apt-get -f -y dist-upgrade

## Preeseed the diskless root with our debconf values so the user
## won't need to interact with the installer
echo 'Constructing preseed'
while read owner variable type value; do
	preseed=("${preseed[@]}" "$variable=$value")
done < /usr/pluto/install/preseed.cfg

preseed=("${preseed[@]}" "debconf/frontend=noninteractive" "debconf/priority=critical")

echo 'Applying preseed'
mkdir -p $TEMP_DIR/tmp/
for setting in "${preseed[@]}"; do
	variable="${setting%%=*}"
	value="${setting#*=}"
	
	echo "export LC_ALL=C"						>  $TEMP_DIR/tmp/script.sh
	echo ". /usr/share/debconf/confmodule"		>> $TEMP_DIR/tmp/script.sh
	echo "db_set \"$variable\" \"$value\"" 		>> $TEMP_DIR/tmp/script.sh
	echo "db_fset \"$variable\" seen true"		>> $TEMP_DIR/tmp/script.sh

	chmod +x $TEMP_DIR/tmp/script.sh
	LC_ALL=C chroot $TEMP_DIR /tmp/script.sh
done

# generate locales
echo 'Generating locales'
echo "en_US.UTF-8 UTF-8" >"$TEMP_DIR"/etc/locale.gen
LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install locales
#chroot "$TEMP_DIR" locale-gen

echo "Installing kernel"
LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install linux-image-diskless 
#LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install linux-image-diskless-${KVER} 
if [[ "$distro" = "intrepid" ]] ; then
  LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install linux-restricted-modules-${KVER}
fi
#LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install linux-ubuntu-modules-${KVER}


########## START CREATE LIST OF DEVICES #################"
## that will run on the md so we will know
## what software to preinstall there
## FIXME: get this list from the database 
DEVICE_LIST="28 62 1759 5 11 1825 26 1808 1901"

# Determine if MythTV is installed by looking for MythTV_Plugin...
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=36"
MythTV_Installed=$(RunSQL "$Q")
if [ $MythTV_Installed ];then
	#MythTV_Plugin is installed, so install MythTV_Player on MD
	DEVICE_LIST="$DEVICE_LIST 35"
fi
########## END CREATE LIST OF DEVICES ###################"


## Prevent lpadmin from running as it blocks the system
LC_ALL=C chroot "$TEMP_DIR" apt-get -y install cupsys-client
cp "$TEMP_DIR"/usr/sbin/lpadmin{,.disabled}
echo > "$TEMP_DIR"/usr/sbin/lpadmin 
LC_ALL=C chroot "$TEMP_DIR" apt-get -y install cups-pdf
mv "$TEMP_DIR"/usr/sbin/lpadmin{.disabled,}


## Prevent discover from running as it blocks the system
## FIXME: install discover1 instead for older distros
LC_ALL=C chroot "$TEMP_DIR" apt-get -y install discover
cp "$TEMP_DIR"/sbin/discover "$TEMP_DIR"/sbin/discover.disabled
echo > "$TEMP_DIR"/sbin/discover

## Begin installing the packages needed for the pluto devices
echo "do_initrd = Yes" > $TEMP_DIR/etc/kernel-img.conf
LC_ALL=C chroot "$TEMP_DIR" invoke-rc.d exim4 force-stop

for device in $DEVICE_LIST; do
	Q="SELECT
		Package_Source.Name
	   FROM
		Package_Source
		JOIN DeviceTemplate on Package_Source.FK_Package = DeviceTemplate.FK_Package
	   WHERE
		PK_DeviceTemplate = $device
	   AND
		FK_RepositorySource = 2					
	"

	R=$(RunSQL "$Q")

	for Row in $R; do
		pkg_name=$(Field 1 "$Row")
	done

	echo "#### Installing $pkg_name for device $device"
	RETURNCODE=1
	while [[ $RETURNCODE != 0 ]]; do
		LC_ALL=C chroot $TEMP_DIR apt-get -y install $pkg_name
		if [[ "$?" == "0" ]] ;then
			echo "#### Package $pkg_name installed ok!"
			RETURNCODE=0
		else
			echo "#### Package $pkg_name failed ($TEMP_DIR) - We wait 10sec and try again - to stop retrying press Ctrl-C. "
			sleep 10s
		fi
	done
done

## Packages that are marked as dependencies only in the database
LC_ALL=C chroot $TEMP_DIR apt-get -y install mozilla-mplayer
LC_ALL=C chroot $TEMP_DIR apt-get -y install id-my-disc

## Put back discover
mv "$TEMP_DIR"/sbin/discover.disabled "$TEMP_DIR"/sbin/discover

## If libdvdcss2 is installed on the hybrid/core
if [[ -d /usr/share/doc/libdvdcss2 ]] ;then
	pushd $TEMP_DIR >/dev/null

#	wget --timeout=10 --tries=4 http://www.dtek.chalmers.se/groups/dvd/deb/libdvdcss2_1.2.5-1_i386.deb
#	chroot $TEMP_DIR dpkg -i /libdvdcss2_1.2.5-1_i386.deb
	chroot $TEMP_DIR apt-get install libdvdcss2
#	rm libdvdcss2_1.2.5-1_i386.deb

	popd
fi

LC_ALL=C chroot $TEMP_DIR apt-get -y install kubuntu-desktop
mv "$TEMP_DIR"/etc/init.d/kdm{,.save}
mv "$TEMP_DIR"/etc/rc2.d/*kdm "$TEMP_DIR"/etc/rc2.d/S99kdm
# Remove whatever 
LC_ALL=C chroot $TEMP_DIR update-rc.d -f kdm remove
LC_ALL=C chroot $TEMP_DIR update-rc.d -f NetworkManager remove
LC_ALL=C chroot $TEMP_DIR apt-get -y install xserver-xorg-video-all
LC_ALL=C chroot $TEMP_DIR apt-get -y install linux-firmware

umount $TEMP_DIR/var/cache/apt
umount $TEMP_DIR/usr/pluto/deb-cache
umount $TEMP_DIR/sys
umount $TEMP_DIR/proc
umount $TEMP_DIR/lib/modules/${KVER}/volatile

mv "$TEMP_DIR"/sbin/start-stop-daemon{.pluto-install,}
mv "$TEMP_DIR"/usr/sbin/invoke-rc.d{.pluto-install,}
rm -f "$TEMP_DIR"/etc/chroot-install
rm -f "$TEMP_DIR"/etc/X11/xorg.conf || :

# Setup Link for the desktop to point to Activate Orbiter
cat <<eol >"$TEMP_DIR"/etc/skel/Desktop/LinuxMCE
[Desktop Entry]
Encoding=UTF-8
Version=8.10
Type=Application
Exec=/usr/pluto/bin/ActivateOrbiterFromKDE.sh
Path=/usr/pluto/bin
Name=Back To LinuxMCE Orbiter
Icon=gnome-panel-launcher
eol

cp -r "$TEMP_DIR"/etc/skel/Desktop/* "$TEMP_DIR"/root/Desktop

#invoke-rc.d nfs-common restart

LC_ALL=C chroot "$TEMP_DIR" apt-get clean
COLUMNS=1024 chroot "$TEMP_DIR" dpkg -l | awk '/^ii/ {print $2}' >/tmp/pkglist-diskless.txt # used for deb-cache cleanup in the builder

mkdir -p "$ARH_DIR"
pushd "$TEMP_DIR" >/dev/null
tar -cjvf "$ARH_DIR/$DisklessFS" *
echo "$PlutoVersion" > "$ARH_DIR/$DisklessFS.version"
popd >/dev/null

rm -rf $TEMP_DIR

# Let's create the default files needed for successful PXE boot.
/usr/pluto/bin/Diskless_BuildDefaultImage.sh

# HACK'O'MATIC And make sure, we have files for both architectures.
if [ -f /usr/pluto/install/PlutoMD-i386.tar.bz2 ]; then
	FILENEEDED=PlutoMD-amd64
else
	FILENEEDED=PlutoMD-i386
fi
if [ ! -f /usr/pluto/install/$FILENEEDED.tar.bz2 ]; then
	ln -sf /usr/pluto/install/$DisklessFS /usr/pluto/install/$FILENEEDED.tar.bz2
fi	
if [ ! -f /usr/pluto/install/$FILENEEDED.tar.bz2.version ]; then
	ln -sf /usr/pluto/install/$DisklessFS.version /usr/pluto/install/$FILENEEDED.tar.bz2.version
fi	

trap - EXIT
