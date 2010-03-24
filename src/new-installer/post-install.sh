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

# Remove KDM startup
echo "Remove softlink to start KDM"
# an alternative would be to disable it in runlevel 5 only
dpkg-divert --add --rename --divert /etc/init.d/kdm.wraped /etc/init.d/kdm

# make sure the kubuntu desktop environment is installed
apt-get install -y kubuntu-desktop

# make sure to install latest alsa drivers
apt-get install -y alsa-modules-`uname -r`

# update-rc.d -f kdm remove

## NOTE: Fix for initramfs-tools - no longer needed - done in mce-install.sh
#patch -Np0 -i mkinitramfs_dot-fix1.patch -r /dev/null >/dev/null 2>&1 || echo "The initramfs-tools dot-problem was already fixed."

# Disable compcache
rm -f /usr/share/initramfs-tools/conf.d/compcache && update-initramfs -u
echo Installing the latest v4l2 modules - If this process takes longer than 1 hour, ctrl-c out. Your LinuxMCE
echo install will work.
# get the latest v4l modules
apt-get install -y v4l2-modules
apt-get install -y v4l2-firmware

#
echo 
echo The first phase of the install process is completed
echo Reboot the system to start the final process.
echo
/usr/share/update-notifier/notify-reboot-required
