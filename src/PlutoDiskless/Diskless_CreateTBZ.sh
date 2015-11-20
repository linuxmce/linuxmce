#!/bin/bash -x

. /usr/pluto/bin/SQL_Ops.sh

## Misc variables
DISTRIB='sarge'
#MIRROR='http://deb.plutohome.com/debian/'
MIRROR='file:/usr/pluto/deb-cache/'
DBST_SCRIPT='/usr/pluto/bin/Diskless_DebootstrapPluto.sh'
ARH_DIR='/usr/pluto/install'
DisklessFS="PlutoMD-pre-install.tar.bz2"

Installer_Files=(BonusCdMenu.sh BonusCdAutoInst.sh Common.sh AptSources.sh ConfirmDependencies_Debian.sh Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh Initial_DHCP_Config.sh Initial_Network_Config.sh pkgsel post-debootstrap.sh preseed.cfg PreseedStage2.sh)

## Create a temporary directory to store the diskless image into and be sure
## it gets removed after we finish
TEMP_DIR=$(mktemp -d)

function trap_EXIT() {
	umount -fl $TEMP_DIR/usr/pluto/deb-cache
	umount -fl $TEMP_DIR/proc
	umount -fl $TEMP_DIR/sys

	rm -rf $TEMP_DIR
}

trap "trap_EXIT" EXIT

## Run debootstrap to create the base debian system for our MD
if [[ 1 == 1 ]]; then
	echo "Creating 'Release' file"
	Release="Origin: Pluto
Label: Pluto
Suite: 20dev
Version: 2.0.0.44
Codename: the_thing
Architectures: i386
Components: main
Description: Pluto Home, Brilliant Living
MD5Sum:"
	pushd /usr/pluto/deb-cache/dists/sarge >/dev/null
	echo "$Release" >Release
	gunzip -c main/binary-i386/Packages.gz >main/binary-i386/Packages
	find main/ -type f -exec md5sum '{}' ';' |
		while read line; do
			MD5sum=$(echo "$line" | awk '{ print $1 }')
			Filename=$(echo "$line" | awk '{ print $2 }')
			Size=$(stat -c '%s' "$Filename")
			printf " %s %16u %s\n" "$MD5sum" "$Size" "$Filename" >>Release
		done
	# end find
	popd >/dev/null
	echo 'Running debootstrap'
	debootstrap $DISTRIB $TEMP_DIR $MIRROR $DBST_SCRIPT
else
	pushd "$TEMP_DIR" >/dev/null
	tar -xjf "$ARH_DIR"/PlutoMD.tar.bz2
	popd >/dev/null
fi

# set up chroot installation environment
touch "$TEMP_DIR"/etc/chroot-install
mv "$TEMP_DIR"/usr/sbin/invoke-rc.d{,.pluto-install}
mv "$TEMP_DIR"/sbin/start-stop-daemon{,.pluto-install}

## Create the temporary sources.list file for the media director 
## FIXME: maybe we need to make it from scratch ?
cp /etc/apt/sources.list $TEMP_DIR/etc/apt

mkdir -p $TEMP_DIR/usr/pluto/deb-cache
mount --bind /usr/pluto/deb-cache $TEMP_DIR/usr/pluto/deb-cache
mount none -t sysfs $TEMP_DIR/sys
mount none -t proc  $TEMP_DIR/proc

echo -en '#!/bin/bash\necho "WARNING: fake invoke-rc.d called"\n' >"$TEMP_DIR"/usr/sbin/invoke-rc.d
echo -en '#!/bin/bash\necho "WARNING: fake start-stop-daemon called"\n' >"$TEMP_DIR"/sbin/start-stop-daemon
chmod +x "$TEMP_DIR"/usr/sbin/invoke-rc.d
chmod +x "$TEMP_DIR"/sbin/start-stop-daemon

cp {,"$TEMP_DIR"}/etc/apt/apt.conf.d/30pluto

echo "pluto" >$TEMP_DIR/etc/defaultdomain

chroot $TEMP_DIR apt-get -y update
chroot $TEMP_DIR apt-get -f -y install
chroot $TEMP_DIR apt-get -f -y dist-upgrade

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
	chroot $TEMP_DIR /tmp/script.sh
done

# generate locales
echo 'Generating locales'
echo "en_US.UTF-8 UTF-8" >"$TEMP_DIR"/etc/locale.gen
LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install locales
#chroot "$TEMP_DIR" locale-gen

## Create a list of devices that will run on the md so we will know
## what software to preinstall there
## FIXME: get this list from the database 
DEVICE_LIST="28 62 1795 5 35 11 1825 26 1808"

## Begin installing the packages needed for the pluto devices
echo "do_initrd = Yes" > $TEMP_DIR/etc/kernel-img.conf
chroot "$TEMP_DIR" invoke-rc.d exim4 force-stop

Branch=3
if [[ "$Branch" == 1 ]]; then # alternative selection
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

	echo "#### Installing $pkg_name"
	chroot $TEMP_DIR apt-get -y install $pkg_name
	if [[ "$?" == "0" ]] ;then
		echo "#### Package $pkg_name installed ok!"
	else
		echo "#### Package $pkg_name failed ($TEMP_DIR)"
		/bin/bash
	fi
done

elif [[ "$Branch" == 2 ]]; then # alternative selection

mkdir -p "$TEMP_DIR"/usr/pluto/install
for File in "${Installer_Files[@]}"; do
	cp /usr/pluto/install/"$File" "$TEMP_DIR"/usr/pluto/install
done

cp {,"$TEMP_DIR"}/usr/pluto/install/PlutoMD-activation.sh
chroot "$TEMP_DIR" bash -x /usr/pluto/install/PlutoMD-activation.sh

elif [[ "$Branch" == 3 ]]; then # alternative selection

PrioPkgs="pluto-boot-scripts"

echo $PrioPkgs | xargs chroot "$TEMP_DIR" apt-get -f -y install
cat /usr/pluto/install/PlutoMD-packages.txt | xargs chroot "$TEMP_DIR" apt-get -f -y install

else

echo "WRONG"
/bin/bash

fi # alternative selection

umount $TEMP_DIR/usr/pluto/deb-cache
umount $TEMP_DIR/sys
umount $TEMP_DIR/proc

mv "$TEMP_DIR"/sbin/start-stop-daemon{.pluto-install,}
mv "$TEMP_DIR"/usr/sbin/invoke-rc.d{.pluto-install,}
rm -f "$TEMP_DIR"/etc/chroot-install

#service nfs-common restart

chroot "$TEMP_DIR" apt-get clean

mkdir -p "$ARH_DIR"
pushd "$TEMP_DIR" >/dev/null
tar -cjvf "$ARH_DIR/$DisklessFS" *
popd >/dev/null

rm -rf $TEMP_DIR

trap - EXIT
