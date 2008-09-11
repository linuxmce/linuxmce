#!/bin/bash

## Decide on the name that the iso will have
iso_name="$1"
if [[ "$iso_name" == "" ]] ;then
	iso_name="lmce-disc"
fi

if [[ "$local_mirror_dir" == "" ]] ;then
	local_mirror_dir="/var/www"
fi

if [[ "$local_cache_cd" == "" ]] ;then
	local_cache_cd="/var/plutobuild/cache-cd"
fi

## Prepare the directory in whith the cd will be built
if [ ! -e $local_cache_cd/deb-cache ] ;then
	mkdir -p $local_cache_cd/deb-cache
else
	rm -f $local_cache_cd/deb-cache/*
	rm -f /var/plutobuild/cache-cd/mce-installer*.deb
fi

## Remove old iso
rm -f $local_mirror_dir/$iso_name-packages.iso

## Copy the debs that where created in $local_mirror_dir on this cd
find $local_mirror_dir/ -maxdepth 1 -type f -name '*.deb' -print0 | xargs -0 -i -t cp '{}' $local_cache_cd/deb-cache


## Remove blacklisted packages from this cd
while read file; do
	rm -f "$local_cache_cd/deb-cache/$file"_*.deb
done < $SVNROOT/src/Ubuntu_Helpers/cd1-packages-blacklist

# Download extra packages for this cd, run automatically only once
if [ ! -e $SVNROOT/build/cd1-packages ]; then
	$SVNROOT/src/Ubuntu_Helpers/cd1-prepare.sh
fi

# Add extra packages to this cd
for pack in $build_root_dir/cd1-packages/*.deb ;do
	cp "$pack" $local_cache_cd/deb-cache
done

## Build a Packages.gz file in the deb-cache dir of the cd
pushd $local_cache_cd/deb-cache
	dpkg-scanpackages . /dev/null > Packages
	cat Packages | gzip -9c > Packages.gz > /dev/null
popd


## Generate the iso
pushd $local_cache_cd/..
	ls_deb=`echo $local_mirror_dir/mce-installer*.deb`
        if [[ "$ls_deb" != "" ]] ; then
		cp $local_mirror_dir/mce-installer*.deb ./cache-cd/ > /dev/null
	else
		echo "Error: mce-installer package missing, cd won't boot!"
	fi
	genisoimage -joliet-long -r -o "${iso_name}-packages.iso" ./cache-cd 
	mv "${iso_name}-packages.iso" $local_mirror_dir > /dev/null
popd

## Clean the temporary dir
rm -rf $local_cache_cd
