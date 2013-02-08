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

# if using GRUB2 (on 1004 core / hybrid) remove kernel command line
if [ -f /etc/default/grub ]; then
        if [[ $(grep 'GRUB_CMDLINE_LINUX_DEFAULT="quiet splash uvesafb mode_option=1024x768-24 mtrr=3 scroll=ywrap"' /etc/default/grub) ]]; then
                sed -i 's/splash uvesafb mode_option=1024x768-24 mtrr=3 scroll=ywrap"/splash"/' /etc/default/grub
        fi
        update-grub
fi

# remove hook from boot log messages
sed -i '/plymouth update/ d' /usr/share/initramfs-tools/scripts/functions

# trigger initrams update to include new theme
dpkg-trigger update-initramfs
