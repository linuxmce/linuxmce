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

echo "*** Done: $0"
