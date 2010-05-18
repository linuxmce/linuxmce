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
pushd ${svn_dir}/${svn_branch_name}/ubuntu/debhelper-7.4.20
      echo "Creating debhelper-7.4.20 debian package"
      dpkg-buildpackage -rfakeroot -b -us -uc
      cp ${svn_dir}/${svn_branch_name}/ubuntu/debhelper*.deb ${replacements_dir}
popd

# Install new debhelper
dpkg -i --force-all ${svn_dir}/${svn_branch_name}/ubuntu/debhelper_7.4.20_all.deb

echo "*** Done: $0"
