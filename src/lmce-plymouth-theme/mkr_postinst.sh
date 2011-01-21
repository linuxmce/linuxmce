#!/bin/bash
#
# LinuxMCE plymouth theme postinstall script

# don't do anything yet until package has matured
exit 0

# Prevent delayed loading of our splash
if [[ ! $(grep "FRAMEBUFFER=y" /etc/initramfs-tools/splash) ]]; then
	echo "FRAMEBUFFER=y" >>/etc/initramfs-tools/conf.d/splash
fi

# register our theme with plymouth
update-alternatives --install /lib/plymouth/themes/default.plymouth default.plymouth /lib/plymouth/themes/LinuxMCE/LinuxMCE.plymouth 900
update-alternatives --config default.plymouth
update-initramfs -u

