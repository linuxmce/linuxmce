#!/bin/bash
#
# LinuxMCE plymouth theme postinstall script

# Prevent delayed loading of our splash
if [[ ! $(grep "FRAMEBUFFER=y" /etc/initramfs-tools/conf.d/splash) ]]; then
	echo "FRAMEBUFFER=y" >>/etc/initramfs-tools/conf.d/splash
fi

# register our theme with plymouth
update-alternatives --install /lib/plymouth/themes/default.plymouth default.plymouth /lib/plymouth/themes/LinuxMCE/LinuxMCE.plymouth 900
update-alternatives --auto default.plymouth

# trigger initrams update to include new theme
dpkg-trigger update-initramfs
