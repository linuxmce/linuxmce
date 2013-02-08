#!/bin/bash

# dirty installing all *.debs :

pushd addons
dpkg -i *.deb
popd
pushd PACKAGES
dpkg -i *.deb
popd
