#!/bin/bash

#echo > /var/log/messages
#echo > /var/log/syslog
#dhclient eth0
ntpdate ntp.ubuntu.com

. ./mce-install-common.sh


if [ ! -e /tmp/mce_wizard_data.sh ]
then
#	cp ./mce_wizard_data.sh /tmp
	if ifconfig -a | grep -q '^eth1'
	then
		cp ./mce_wizard_data-double_nic.sh /tmp/mce_wizard_data.sh
	else
		cp ./mce_wizard_data-single_nic.sh /tmp/mce_wizard_data.sh
	fi
fi

rm -rf /tmp/mce_installer_error
InstallerLogFile="/var/log/mce-installer-$(date +%Y%m%d_%H%M%S).log"
exec &> >(tee $InstallerLogFile)

echo "--- start mce wizard data ---" 
cat /tmp/mce_wizard_data.sh 
echo "--- end mce wizard data ---"

if [[ ! -r /tmp/mce_wizard_data.sh ]] ;then
        ExitInstaller "Wizard Information is corrupted or missing."
fi
. /tmp/mce_wizard_data.sh


Core_PK_Device="0"

if [[ "$c_netExtKeep" != "true" ]] ;then
	Setup_Network_Intefaces
fi

Setup_NIS

#trap 'RemoveOfflineSource' EXIT
#Setup_Apt_Conffiles
Setup_Pluto_Apt_Conf
./mce-install-preseed.sh
apt-get update

#FakeUpdates

Setup_Pluto_Conf

Install_DCERouter

Create_And_Config_Devices

Configure_Network_Options

Setup_XOrg

/usr/pluto/bin/SetupUsers.sh

/usr/pluto/bin/Timezone_Detect.sh

/usr/pluto/bin/Network_Setup.sh

# NOTE: Fix for initramfs-tools
patch -Np0 -i mkinitramfs_dot-fix1.patch -r /dev/null >/dev/null 2>&1 || echo "The initramfs-tools dot-problem was already fixed."

StatsMessage "Building a disk image for your Diskless Media Directors"
/usr/pluto/bin/Diskless_CreateTBZ.sh

#mkdir -p /usr/pluto/deb-cache

MangleStartupFiles

ClearInstallationTable

apt-get -y -f dist-upgrade

StatsMessage "Installation Finished"

