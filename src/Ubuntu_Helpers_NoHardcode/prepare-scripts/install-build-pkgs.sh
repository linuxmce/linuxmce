#!/bin/bash

# Install packages
apt-get -f install `cat /etc/lmce-build/build-packages`

# Unpack kernel source
pushd /usr/src
	tar jxvf linux-source-`uname -r | cut -d '-' -f1`.tar.bz2
popd
