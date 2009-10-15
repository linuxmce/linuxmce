#!/bin/bash

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

echo Create link to our new startup script
ln -s /etc/init.d/linuxmce /etc/rc5.d/S93linuxmce

## NOTE: Fix for initramfs-tools - no longer needed - done in mce-install.sh
#patch -Np0 -i mkinitramfs_dot-fix1.patch -r /dev/null >/dev/null 2>&1 || echo "The initramfs-tools dot-problem was already fixed."

#
echo 
echo The first phase of the install process is completed
echo Reboot the system to start the final process.
echo
/usr/share/update-notifier/notify-reboot-required