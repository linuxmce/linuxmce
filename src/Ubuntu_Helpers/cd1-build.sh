#!/bin/bash

## Decide on the name that the iso will have
iso_name="$1"
if [[ "$iso_name" == "" ]] ;then
	iso_name="linuxmce-1.1"
fi

## Prepare the directory in whith the cd will be built
if [ ! -e /var/plutobuild/cache-cd/deb-cache ] ;then
	mkdir -p /var/plutobuild/cache-cd/deb-cache
else
	rm -f /var/plutobuild/cache-cd/deb-cache/*
	rm -f /var/plutobuild/cache-cd/mce-installer*.deb
fi

## Remove old iso
rm -f /var/www/$iso_name-packages.iso

## Copy the debs that where created in /var/www on this cd
find /var/www/ -maxdepth 1 -type f -name '*.deb' -print0 | xargs -0 -i -t cp '{}' /var/plutobuild/cache-cd/deb-cache


## Remove blacklisted packages from this cd
while read file; do
	rm -f "/var/plutobuild/cache-cd/deb-cache/$file"_*.deb
done </root/Ubuntu_Helpers/cd1-packages-blacklist

## Add some extra packages to this cd
for pack in /var/plutobuild/cd1-packages/*.deb ;do
	cp "$pack" /var/plutobuild/cache-cd/deb-cache
done

## Build a Packages.gz file in the deb-cache dir of the cd
pushd /var/plutobuild/cache-cd/deb-cache
	dpkg-scanpackages . /dev/null > Packages
	cat Packages | gzip -9c > Packages.gz
popd


## Generate the iso
pushd /var/plutobuild/
	cp /var/www/mce-installer*.deb ./cache-cd/
	mkisofs -f -joliet-long -r -o "${iso_name}-packages.iso" ./cache-cd
	mv "${iso_name}-packages.iso" /var/www
popd

## Clean the temporary dir
rm -rf /var/plutobuild/cache-cd
