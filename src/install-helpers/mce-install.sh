#!/bin/bash

set -x

export LC_ALL="C"
export DEBIAN_FRONTEND=noninteractive

# update the date
date -s "$(wget -qSO- --max-redirect=0 google.com 2>&1 | grep Date: | cut -d' ' -f5-8)Z"

#create preseed file
cat <<-EOF >/tmp/preseed.cfg
debconf debconf/frontend select  Noninteractive
# Choices: critical, high, medium, low
debconf debconf/priority select  critical
msttcorefonts   msttcorefonts/http_proxy        string
msttcorefonts   msttcorefonts/defoma    note
msttcorefonts   msttcorefonts/dlurl     string
msttcorefonts   msttcorefonts/savedir   string
msttcorefonts   msttcorefonts/baddldir  note
msttcorefonts   msttcorefonts/dldir     string
msttcorefonts   msttcorefonts/blurb     note
msttcorefonts   msttcorefonts/accepted-mscorefonts-eula boolean true
msttcorefonts   msttcorefonts/present-mscorefonts-eula  boolean false
sun-java6-bin   shared/accepted-sun-dlj-v1-1    boolean true
sun-java6-jre   shared/accepted-sun-dlj-v1-1    boolean true
sun-java6-jre   sun-java6-jre/jcepolicy note
sun-java6-jre   sun-java6-jre/stopthread        boolean true
EOF
debconf-set-selections /tmp/preseed.cfg

# update new software pkgs
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" dist-upgrade

#Install required packages
#apt update
DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" install iproute2 ifupdown net-tools ntpdate bind9

# Remove net-tools to prevent old command usage
# Cannot do this yet, we use a combination of net-tools and iproute2
#DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" remove net-tools

#blacklist the sources.list file for now
#echo "/etc/apt/sources.list" >> /etc/confblacklist

# Remove the apt proxy file
mv /etc/apt/apt.conf.d/02proxy ~/ && apt-get update || :

TARGET_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
TARGET_RELEASE=$(lsb_release -c -s)
REPO="main"

echo >>/etc/apt/sources.list
echo "deb http://deb.linuxmce.org/${TARGET_DISTRO}/ ${TARGET_RELEASE} ${REPO}" >>/etc/apt/sources.list

apt-get -q update
###echo DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" install lmce-core | tee /var/log/LinuxMCE-install.log
DEBIAN_FRONTEND="noninteractive" apt-get -y -q --allow-unauthenticated install lmce-hybrid | tee $(basename $0 | cut -d'.' -f1).log



###########################################################
###########################################################

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/install/install-core.sh

Nic_Config # setup initial nic config
Configure_Network_Options # determine ip & int/ext if
Configure_Network_Files # write /etc/network/interfaces /etc/hosts

ifup -a || :
TimeUpdate
Configure_NTP_Server
# Do not create and config under ubiquity.
if ! grep ubiquity /proc/cmdline ; then
        Create_And_Config_Core # create the core device in the database
        Configure_Network_Database # write the network settings to the database
        /usr/pluto/bin/Network_Setup.sh # setup all networking related services
fi
Configure_SSH_Server
Configure_SSH_Client

# Disable NetworkManager and systemd-networkd. TODO: move to systemd-netword?
systemctl disable NetworkManager
systemctl disable systemd-networkd
