#!/bin/bash

#echo > /var/log/messages
#echo > /var/log/syslog
#dhclient eth0


ntpdate ntp.ubuntu.com

. ./mce-install-common.sh

rm -rf /tmp/mce_installer_error
InstallerLogFile="/var/log/mce-installer-$(date +%Y%m%d_%H%M%S).log"
exec &> >(tee $InstallerLogFile)

Core_PK_Device="0"

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

