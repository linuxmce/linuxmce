#!/bin/bash
. /usr/pluto/bin/nvidia-install.sh

source post-install-common.sh

# install the correct nvidia driver
installCorrectNvidiaDriver

CleanCoreSetup

FixAsteriskConfig

addAdditionalTTYStart

#To please the "PlutoRestorePoint.sh" script.
mkdir -p /home/backup/upload

# make sure the kubuntu desktop environment is installed
apt-get install -y --force-yes kubuntu-desktop

# make sure to install latest alsa drivers
if [[ "$DISTRO" = "lucid" ]] ; then
	echo "NOT installing alsa modules on lucid"	
else
	apt-get install -y --force-yes alsa-modules-`uname -r`
fi

## NOTE: Fix for initramfs-tools - no longer needed - done in mce-install.sh
#patch -Np0 -i mkinitramfs_dot-fix1.patch -r /dev/null >/dev/null 2>&1 || echo "The initramfs-tools dot-problem was already fixed."

# Disable compcache
rm -f /usr/share/initramfs-tools/conf.d/compcache && update-initramfs -u
echo Installing the latest v4l2 modules 
# get the latest v4l modules
if [[ "$DISTRO" = "lucid" ]] ; then
	echo "NOT installing v4l modules and firmware on lucid"
else
	apt-get install -y --force-yes v4l2-modules
	apt-get install -y --force-yes v4l2-firmware
fi

# Remove KDM startup
echo "Prevent KDM from starting"
echo "/bin/false" >/etc/X11/default-display-manager

cp firstboot /etc/rc5.d/S90firstboot

## Disable KMS
#(
#echo options nouveau modeset=0
#echo options radeon modeset=0
#echo options i915 modeset=0
#) > /etc/modprobe.d/disable-kms.conf

## Put a temporary nis config file that will prevent ypbind to start
echo "
NISSERVER=false
NISCLIENT=false
YPPWDDIR=/etc
YPCHANGEOK=chsh
NISMASTER=
YPSERVARGS=
YPBINDARGS=
YPPASSWDDARGS=
YPXFRDARGS=
" > /etc/default/nis

#


echo >> /etc/apt/sources.list
echo 
echo The first phase of the install process is completed
echo Reboot the system to start the final process.
echo
/usr/share/update-notifier/notify-reboot-required
