#!/bin/bash
. /usr/pluto/bin/Utils.sh

source post-install-common.sh


CleanCoreSetup

FixAsteriskConfig

addAdditionalTTYStart

#To please the "PlutoRestorePoint.sh" script.
mkdir -p /home/backup/upload

# Remove KDM startup
echo "Remove softlink to start KDM"
# an alternative would be to disable it in runlevel 5 only
update-rc.d -f kdm remove

## NOTE: Fix for initramfs-tools - no longer needed - done in mce-install.sh
#patch -Np0 -i mkinitramfs_dot-fix1.patch -r /dev/null >/dev/null 2>&1 || echo "The initramfs-tools dot-problem was already fixed."

# Disable compcache
rm -f /usr/share/initramfs-tools/conf.d/compcache && update-initramfs

# install the correct nvidia driver
installCorrectNvidiaDriver

#
echo 
echo The first phase of the install process is completed
echo Reboot the system to start the final process.
echo
/usr/share/update-notifier/notify-reboot-required
