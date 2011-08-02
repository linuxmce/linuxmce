#!/bin/bash

#echo > /var/log/messages
#echo > /var/log/syslog
#dhclient eth0


ntpdate ntp.ubuntu.com

. ./mce-install-common.sh

rm -rf /tmp/mce_installer_error
InstallerLogFile="/var/log/mce-installer-$(date +%Y%m%d_%H%M%S).log"
exec &> >(tee $InstallerLogFile)
# Fix repository URL for intrepid
./repoplaces.sh
echo >> /etc/apt/sources.list
# Fix a problem with the /initrd.img and /vmlinuz links pointing to a different kernel than the 
# newest (and currently running) one
ln -s -f /boot/initrd.img-`uname -r` /initrd.img
ln -s -f /boot/vmlinuz-`uname -r` /vmlinuz

# Find out, what nic configuration we have. This is needed for later on to fill the database
# correctly.
#if [ ! -e /tmp/mce_wizard_data.sh ]
#then
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
#fi

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

Setup_Network_Intefaces
cat /etc/network/interfaces

# Temporary NIS setup, disabling NIS server and client.
Setup_NIS

# Setup apt.conf.d for pluto, which allows unauthenticated packages and 
# make sure to keep the old configuration file (without asking)
Setup_Pluto_Apt_Conf

# Pre-seed some answers into the dpkg database, like accepting the Java 
# license, the ivtv license, and telling debconf to stay out of our way.
./mce-install-preseed.sh
apt-get update

#FakeUpdates

# Define the default /etc/pluto.conf, with Mysql details, router port etc.
Setup_Pluto_Conf

# Setup the dcerouter package. Before that, mysql-server needs to installed, configuration
# for mysql needs to be amended, the pluto-sample media package is being installed
# as well as the video-wizard-videos (Sarah..)
#
# Installing the pluto-dcerouter package will autoinstall all the other requirements for 
# the router, like:
#  Depends: pluto-sample-media = contains some sample media, not really needed
#           pluto-confirm-dependencies = installs needed packages for devices
#           pluto-createdevice == creates devices based on device templates
#           pluto-dcecommon == contains the DCE library
#           pluto-dhcpd-plugin == contains engine which monitors the DHCP log for network pnp
#           pluto-install-scripts == scripts for installing pluto (are they still needed)?
#           pluto-local-database == sqlCVS dump coontaining alterations for the local pluto_main database
#           pluto-messagesend == command line messagesend utility.
#           pluto-newmdinteractor == needed to setup files for a new MediaDirector
#           pluto-orbitergen == creates the bitmap files for the Orbiter
#           pluto-orbiterinstaller == Contains the installer files for Window orbiters
#           pluto-plutodhcp == Creates DHCP configuration file
#           pluto-plutoutils == Helper libs for lots of pluto progs.
#           pluto-pluto-main-db == pluto_main database dump from sqlCVS
#           pluto-qos == Perl script setting up the firewall
#           pluto-remoteassistance == Remote assistance package (not working atm)
#           pluto-serializeclass == lib to serialize data
#           pluto-skins-basic == bitmaps for the basic skin
#           pluto-std-plugins == contains all the default plugins running in the dcerouter mem space (datagrid,event,infrared,plug'n'play,file_grid,security,climate,orbiter,media,telecom,lighting and general info plugin
#           pluto-system-database == sqlCVS dumps of the dce,designer,constants,ir,website repos, as well as the city dump containing location information.
#           pluto-updateentarea == creates default scenarios for entertainment areas
#           pluto-website-admin == web site, and web orbiter scripts
#           pluto-windowsce-libraries == DLLs needed for the old windows ce tools (?)
#           pluto-boot-scripts == all the scripts pertaining to the bootprocess of LinuxMCE. Lots of those scripts are probably no longer in use.
#           mce-diskless-tools == Diskless image blue print, as well as script files to create new default images
#           lmce-skins-titanium == bitmaps of the titanium skin
#           lmce-skins-slate == bitmaps of the slate skin

Install_DCERouter

# Create the initial core device using CreateDevice, and the MD for the core in case we create a Hybrid (the default).
Create_And_Config_Devices

# Updating hosts file and the Device_Data for the core with the internal and external network
# addresses - uses Initial_DHCP_Config.sh from the pluto-install-scripts package.
Configure_Network_Options

StatsMessage "Building a disk image for your Diskless Media Directors"
# /usr/pluto/bin/Diskless_CreateTBZ.sh

#mkdir -p /usr/pluto/deb-cache

# This is no longer done.
# MangleStartupFiles

# Do some cleanup to remove the installer icons from the user desktops.
RemoveInstallerIcons

# We now set the installation number to 1, update the description of the install with todays date, and 
# reset all the country and address information, which is kinda stupid after first filling the information uding
# timezone detect...
#ClearInstallationTable

##
apt-get -y -f --force-yes dist-upgrade
VerifyExitCode "dist-upgrade"

StatsMessage "Installation Finished"

