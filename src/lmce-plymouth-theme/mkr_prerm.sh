#!/bin/bash
#
# LinuxMCE plymouth theme preremove script

# remove our theme with plymouth
update-alternatives --remove default.plymouth /lib/plymouth/themes/LinuxMCE/LinuxMCE.plymouth
update-alternatives --auto default.plymouth

# trigger initrams update to include new theme
dpkg-trigger update-initramfs
