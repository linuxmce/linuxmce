#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh

set -e

echo ""
echo "********************************************************************************"
echo "*** Running: $0"
echo "********************************************************************************"

# Install packages
apt-get -q -f -y --force-yes install `cat $lmce_build_conf_dir/build-packages`

# Unpack kernel source
pushd /usr/src
	echo "Unpacking kernel source linux-source-"$KVER_SHORT".tar.bz2"
	tar xjf linux-source-"$KVER_SHORT".tar.bz2
popd

# Build new debhelper package
pushd /usr/src
      # download new file
      wget http://ftp.de.debian.org/debian/pool/main/d/debhelper/debhelper_7.4.20.tar.gz
      echo "Unpacking debhelper_7.4.20.tar.gz"
      tar -zxvf debhelper_7.4.20.tar.gz
      echo "Creating debhelper-7.4.20 debian package"
      mv debhelper debhelper_7.4.20
      cd debhelper_7.4.20
      sed -i 's/--utf8//' Makefile
      dpkg-buildpackage -rfakeroot -b -us -uc
      echo "Installing debhelper-7.4.20 debian package"
      dpkg -i --force-all /usr/src/debhelper_7.4.20_all.deb
popd

# Install packages needed for mythtv .23 build
apt-get -q -f -y --force-yes install yasm libfaac-dev libfribidi-dev liba52-0.7.4-dev
# Remove packages that are conflicts with mythtv .23 build
dpkg -r libmyth-dev qt3-dev-tools libqt3-mt-dev libartsc0-dev

echo "*** Done: $0"
