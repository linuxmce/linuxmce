#!/bin/bash

TARGET_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
TARGET_RELEASE=$(lsb_release -c -s)
REPO="main"

echo >>/etc/apt/sources.list
echo "deb http://deb.linuxmce.org/${TARGET_DISTRO}/ ${TARGET_RELEASE} ${REPO}" >>/etc/apt/sources.list

apt-get -q update
apt-get -y -q --allow-unauthenticated install lmce-install-scripts

. /usr/pluto/install/install-core.sh

UpdateDebCache
ConfigSources
gpgUpdate

apt-get -y -q --allow-unauthenticated install lmce-hybrid
