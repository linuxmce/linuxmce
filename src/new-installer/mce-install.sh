#!/bin/bash

#echo > /var/log/messages
#echo > /var/log/syslog
#dhclient eth0
ntpdate ntp.ubuntu.com

. ./mce-install-common.sh

if [ ! -e /tmp/mce_wizard_data.sh ]
then
	if  [[ `ifconfig -s -a  | awk '$1 != "Iface" && $1 != "lo" && $1 != "pan0" { print $1 }' | wc -l` > 1 ]]
	then
		cp ./mce_wizard_data-double_nic.sh /tmp/mce_wizard_data.sh
		#Use these for the defaults if we cannot automatically determine which to use
		#TODO: Error out and instruct the user to setup a working connection? Or ask them to manually choose?
		extif="eth0"
		intif="eth1"
		if route -n | grep -q '^0.0.0.0'
		then
			#We have a default route, use it for finding external interface.
			extif=`route -n | awk '$1 == "0.0.0.0" { print $8 }'`
			#Use the first available interface as the internal interface.
		  	for if in `ifconfig -s -a | awk '$1 != "Iface" && $1 != "lo"  && $1 != "pan0" { print $1 }'`
		  	do
				if [ "$if" != "$extif" ]
			 	then
					intif=$if
					break
			 	fi
		  	done
		fi
		echo "Using $extif for external interface"
		echo "Using $intif for internal interface"
		
		sed --in-place -e "s,\({extif}\),$extif,g" /tmp/mce_wizard_data.sh
		sed --in-place -e "s,\({intif}\),$intif,g" /tmp/mce_wizard_data.sh
	else
		extif=eth0
		if route -n | grep -q '^0.0.0.0'
                then
                	#We have a default route, use it for finding external interface.
                	extif=`route -n | awk '$1 == "0.0.0.0" { print $8 }'`
		fi
		cp ./mce_wizard_data-single_nic.sh /tmp/mce_wizard_data.sh
		echo "Using $extif for single nic install"
		sed --in-place -e "s,\({extif}\),$extif,g" /tmp/mce_wizard_data.sh

                # set c_netExtIP and friends , as this is used in Configure_Network_Options (i.e. before Network_Setup...)
                extIP=$(ip addr | grep "$extif" | grep -m 1 'inet ' | awk '{print $2}' | cut -d/ -f1)
                sed --in-place -e "s,\({extip}\),$extIP,g" /tmp/mce_wizard_data.sh

                # Set use external DHCP and run own dhcp based on extifs current setting
                ExtUsesDhcp=$(grep "iface $extif " /etc/network/interfaces | grep -cF 'dhcp')
		if [[ $ExtUsesDhcp == 0 ]]
                then
		       # Not dhcp defined in config file, test if dhclient got us an IP
		       # /var/run/dhcp3 for newer than 810, /var/run in 810
		       if [[ (-e /var/lib/dhcp3/dhclient-$extif.lease || -e /var/run/dhclient-$extif.lease) && `pgrep -c dhclient` == 1 ]]
		       then
			       ExtUsesDhcp=1
		       fi
		fi
                RunDHCP=0
                if [[ $ExtUsesDhcp == 0 ]]
                then
                        echo "$extif does not use DHCP, setting ExtUseDhcp=0 and RunDHCPServer=1 and detecting current network settings"
                        RunDHCP=1

                        ExtGateway=$(grep -A 10 "^iface $extif" /etc/network/interfaces | grep '^\s*gateway' -m 1 | grep -o  '[0-9.]*')
                        ExtMask=$(grep -A 10 "^iface $extif" /etc/network/interfaces | grep '^\s*netmask' -m 1 | grep -o '[0-9.]*')
                        ExtDNS=$(grep 'nameserver' /etc/resolv.conf | grep -o '[0-9.]*' -m 1)
		fi
                
		sed --in-place -e "s,\({extMask}\),$ExtMask,g" /tmp/mce_wizard_data.sh
		sed --in-place -e "s,\({extGW}\),$ExtGateway,g" /tmp/mce_wizard_data.sh
		sed --in-place -e "s,\({extDNS}\),$ExtDNS,g" /tmp/mce_wizard_data.sh

		sed --in-place -e "s,\({extUseDhcp}\),$ExtUsesDhcp,g" /tmp/mce_wizard_data.sh
		sed --in-place -e "s,\({runDhcp}\),$RunDHCP,g" /tmp/mce_wizard_data.sh
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

RemoveInstallerIcons

ClearInstallationTable

apt-get -y -f dist-upgrade

# Fix apt proxy settings so it apply to core and MDs after lmce is configured.
if [ -f /etc/apt/apt.conf.d/02proxy ]; then
	sed -i 's@localhost:3142@dcerouter:3142@' /etc/apt/apt.conf.d/02proxy
fi
StatsMessage "Installation Finished"

