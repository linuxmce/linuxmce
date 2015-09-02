#!/bin/bash

DISTRO="$(lsb_release -c -s)"
COMPOS="main"

echo >>/etc/apt/sources.list
echo "deb http://deb.linuxmce.org/ubuntu/ ${DISTRO} ${COMPOS}" >>/etc/apt/sources.list

apt-get -q update
apt-get -q --allow-unauthenticated install lmce-install-scripts

. /usr/pluto/install/install-core.sh

UpdateDebCache
ConfigSources
gpgUpdate

apt-get -q --allow-unauthenticated install lmce-hybrid
