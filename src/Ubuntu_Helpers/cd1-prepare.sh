#!/bin/bash

rm -rf /var/plutobuild/cd1-packages
mkdir -p /var/plutobuild/cd1-packages

pushd /var/plutobuild/cd1-packages
	aptitude download `cat /root/Ubuntu_Helpers/cd1-packages-whitelist.gutsy`
popd

