#!/bin/bash
#
# LinuxMCE plymouth theme postinstall script

# Prevent delayed loading of our splash
if [ -f /etc/initramfs-tools/conf.d/splash ]; then
	if [[ ! $(grep "FRAMEBUFFER=y" /etc/initramfs-tools/conf.d/splash) ]]; then
		echo "FRAMEBUFFER=y" >>/etc/initramfs-tools/conf.d/splash
	fi
else
	echo "FRAMEBUFFER=y" >/etc/initramfs-tools/conf.d/splash
fi

# nVidia proprietary drivers lack fb support, use uvesafb instead
if [[ ! $(grep "uvesafb mode_option=1024x768-24 mtrr=3 scroll=ywrap" /etc/initramfs-tools/modules) ]]; then
	echo "uvesafb mode_option=1024x768-24 mtrr=3 scroll=ywrap" >>/etc/initramfs-tools/modules
fi

# blacklist vga16fb fallback to be sure uvesafb is used
if [[ ! $(grep "blacklist vga16fb" /etc/modprobe.d/blacklist-framebuffer.conf) ]]; then
        echo "blacklist vga16fb" >>/etc/modprobe.d/blacklist-framebuffer.conf
fi

# hook into funtions to show boot log messages
if [[ ! $(grep '/bin/plymouth update --status="Begin: $@"' /usr/share/initramfs-tools/scripts/functions) ]]; then
	sed -i '/Begin:/ a\
	/bin/plymouth update --status="Begin: $@"' /usr/share/initramfs-tools/scripts/functions
fi

# if using GRUB2 (on 1004 core / hybrid) add kernel command line
if [ -f /etc/default/grub ]; then
        if [[ $(grep 'GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"' /etc/default/grub) ]]; then
                sed -i 's/splash"/splash uvesafb mode_option=1024x768-24 mtrr=3 scroll=ywrap"/' /etc/default/grub
        fi
	if [[ ! $(hostname|grep moon) ]]; then
		update-grub
	fi
fi

# register our theme with plymouth
update-alternatives --install /lib/plymouth/themes/default.plymouth default.plymouth /lib/plymouth/themes/LinuxMCE/LinuxMCE.plymouth 900
update-alternatives --auto default.plymouth

# trigger initrams update to include new theme
dpkg-trigger update-initramfs
