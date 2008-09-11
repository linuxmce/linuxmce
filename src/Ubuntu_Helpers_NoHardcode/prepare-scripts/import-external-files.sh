#!/bin/bash -e

. /etc/lmce-build/builder.conf

rsync_host="10.0.2.3"

## Import Public Skin
skins_dir=/home/samba/www_docs/graphics
mkdir -p /usr/pluto/orbiter/
mkdir -p "$skins_dir"

rm -f /usr/pluto/orbiter/skins
ln -s "$skins_dir" /usr/pluto/orbiter/skins

pushd "$skins_dir"
	rsync -avz --delete-after rsync://$rsync_host/skins-public ./
popd

pushd /usr/pluto/orbiter/skins
        cp -r LinuxMCE/* Basic/
popd

pushd "${build_dir}"
	rm -f samba
        ln -s /home/samba
popd
	
mkdir -p '/home/samba/www_docs/sample media/'
pushd '/home/samba/www_docs/sample media/'
	rsync -avz --delete-after rsync://$rsync_host/sample-media ./
popd
