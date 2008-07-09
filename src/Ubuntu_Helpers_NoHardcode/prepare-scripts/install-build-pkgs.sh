#!/bin/bash

. /usr/local/lmce-build/common/env.sh

# Install packages
apt-get -f install `cat $lmce_build_conf_dir/build-packages`

# Unpack kernel source
pushd /usr/src
	tar jxvf linux-source-"$KVER_SHORT".tar.bz2
popd
