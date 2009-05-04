#!/bin/bash

source post-install-common.sh


CleanCoreSetup

FixAsteriskConfig

addAdditionalTTYStart

#To please the "PlutoRestorePoint.sh" script.
mkdir -p /home/backup/upload


## NOTE: Fix for initramfs-tools - no longer needed - done in mce-install.sh
#patch -Np0 -i mkinitramfs_dot-fix1.patch -r /dev/null >/dev/null 2>&1 || echo "The initramfs-tools dot-problem was already fixed."

