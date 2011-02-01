#!/bin/bash
#
# LinuxMCE plymouth theme preremove script

# remove our theme with plymouth
update-alternatives --remove default.plymouth /lib/plymouth/themes/LinuxMCE/LinuxMCE.plymouth
update-alternatives --auto default.plymouth

# remove vga16fb from blacklisted file
sed -i '/vga16fb/ d' /etc/modprobe.d/blacklist-framebuffer.conf

# remove uvesafb from modules
sed -i '/uvesafb/ d' /etc/initramfs-tools/modules

# trigger initrams update to include new theme
dpkg-trigger update-initramfs
