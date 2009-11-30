#!/bin/bash
# SourceIso contains the name of the ISO file, which MUST reside in the live directory
# BUILD_ROOT is a link to the base of the builder environement
#
# This procedure creates a live dir under the users homedir. This directory is recreated 
# each time this script runs.
#
# At the end of this script, you need to run finalizedvd.sh which adds additional information,
# and creates the iso.
#
# Copyright 2009 Peer Oliver Schmidt
# License: GPL v3
#
# In part copied from various places on the web.


SourceIso=kubuntu-8.10-dvd-i386.iso
SourceIso=kubuntu-8.10-desktop-i386.iso
# SourceIso=karmic-desktop-i386.iso
BUILDER_ROOT=/opt/builder386
SVNrevision=$(svn info "$BUILDER_ROOT/var/lmce-build/svn/branches/LinuxMCE-0810/src" |grep ^Revision | cut -d" " -f2)

echo Aktuell svn Revision $SVNrevision

echo Make sure rsync, squashfs-tools and mkisofs is installed
sudo apt-get install rsync squashfs-tools genisoimage -y
#rm -fR ~/live-old
#mv ~/live ~/live-old
mkdir -p ~/live
# mv $SourceIso ~/live
pushd ~/live
sudo umount {squashfs,mnt}
echo "Deleting old contents"
rm -fR ~/live/*
# 

echo Extracting the CD contents

echo Mount the Desktop .iso

mkdir -p mnt
sudo mount -o loop ../$SourceIso mnt

echo Extract .iso contents into dir 'extract-cd'

mkdir -p extract-cd
rsync --exclude=/casper/filesystem.squashfs -av mnt/ extract-cd

echo Extracting the Desktop system

echo Mount the squashfs filesystem

mkdir -p squashfs
sudo mount -t squashfs -o loop mnt/casper/filesystem.squashfs squashfs

echo Extract squashfs contents into dir 'edit'

mkdir -p edit
sudo cp -a squashfs/* edit/

echo Prepare and chroot

echo If you need the network connection within chroot

sudo cp /etc/resolv.conf edit/etc/

echo Depending on your configuration, you may also need to copy the hosts file

sudo cp /etc/hosts edit/etc/
#echo "Acquire::http::Proxy \"http://127.0.0.1:3128\";">edit/etc/apt/apt.conf
sudo mount -t proc none ~/live/edit/proc

sudo mount --bind /dev edit/dev
sudo mount none edit/dev/pts -t devpts

echo "Get Installer from build environment"
cp -r $BUILDER_ROOT/var/lmce-build/svn/branches/LinuxMCE-0810/src/new-installer edit/root/
# 
# We disable the creation of the Diskless base during DVD install
#
sed '/Diskless_CreateTBZ.sh/d' -i edit/root/new-installer/mce-install.sh
cat <<eol > edit/root/new-installer/full-install.sh
#!/bin/bash
echo "Starting mce-install.sh"
bash mce-install.sh
echo "Running post-install.sh"
bash post-install.sh
echo "Done!"
eol
chmod +x edit/root/new-installer/full-install.sh

echo "Copying over the current debs"
DEST=edit
mkdir -p $DEST/usr/pluto/deb-cache
cp $BUILDER_ROOT/var/www/{*.deb,Package*,Release*} $DEST/usr/pluto/deb-cache
# Get the saved alsa files 
cp /var/www/*.deb $DEST/usr/pluto/deb-cache
# Temporarily remove the biggest files.
rm $DEST/usr/pluto/deb-cache/lmce-mame-metadata*
#{video-wizard-videos*,pluto-sample-media*}
# ,

echo "Updating packages file"
pushd  $DEST/usr/pluto/deb-cache
# Generate the Packages files
wget http://deb.linuxmce.org/ubuntu/dists/intrepid/lmce-alpha2/binary-all/libft-perl_1.0_all.deb 
wget http://deb.linuxmce.org/ubuntu/dists/intrepid/lmce-alpha-latest/binary-all/mail-transport-agent_1.0_all.deb 
wget http://deb.linuxmce.org/ubuntu/dists/intrepid/lmce-alpha-latest-updates/binary-i386/id-my-disc_1.8.ub0710_i386.deb
echo  "Generating Packages / Packages.gz files"
dpkg-scanpackages . /dev/null > Packages
cat Packages | gzip -9c > Packages.gz
popd
                                                                

echo Now updating the install
echo Creating an upgrade script
cp ~/installed-programs edit/root/
cp ~/selection edit/root/
cat <<eol >edit/root/upgrade.sh
#!/bin/bash
export LC_ALL=C
export DEBIAN_FRONTEND=noninteractive
export http_proxy="http://127.0.0.1:3128/"
apt-get update
apt-get dist-upgrade -dy
apt-get autoremove
dpkg --configure -a --abort-after 20000
# We run some pre-installer- which sets up sources.list among other things
cd /root/new-installer
bash ./mce-install-preseed.sh
bash ./pre-install-from-DVD.sh
echo deb http://fluffybitch.org/build/ ./>>/etc/apt/sources.list
apt-get update
apt-get dist-upgrade -y --force-yes
apt-get install -y --force-yes mplayer
apt-get install -y --force-yes libdancer-xml0
apt-get -dy --force-yes install pluto-dcerouter
# Install the nVidia drivers
apt-get install -y --force-yes nvidia-glx-195 nvidia-195-modaliases nvidia-195-libvdpau

# I want joe
apt-get install -y joe

# Install festival voices
apt-get install -y --force-yes festival festival-czech festival-hi festival-mr festival-te festvox-czech-ph festvox-don festvox-ellpc11k festvox-hi-nsk festvox-italp16k festvox-itapc16k festvox-kallpc16k festvox-kdlpc16k festvox-mr-nsk festvox-rablpc16k festvox-suopuhe-common festvox-suopuhe-lj festvox-suopuhe-mv festvox-te-nsk 
# pre Install Diskless tools
apt-get install -y --force-yes mce-diskless-tools
pushd /usr/pluto/bin
# ./Diskless_CreateTBZ.sh
echo Done!
popd
# Now we setup the package selection that we'd like to have
wget http://deb.linuxmce.org/ubuntu/kubuntu810desktop-selection
# Add the packages that are already installed
dpkg --get-selections >> /root/selection
# kubuntu810desktop-selection
sort /root/selection > /root/test.selection
# and import the whole shebang so everything is marked
dpkg --set-selections < /root/test.selection
# And get all the stuff downloaded.
apt-get dselect-upgrade -dy --force-yes
eol

chmod +x edit/root/upgrade.sh
sudo chroot edit /root/upgrade.sh

echo Creating Desktop button
mkdir -p edit/etc/skel/Desktop
cat <<eol >edit/etc/skel/Desktop/LinuxMCE
[Desktop Entry]
Encoding=UTF-8
Version=8.10
Type=Application
Exec=kdesudo /root/new-installer/full-install.sh
Path=/root/new-installer
Name=LinuxMCE Installer
Icon=gnome-panel-launcher
Terminal=true
TerminalOptions=\s--noclose
eol


umount -l edit/{dev/pts,dev,proc,sys}
umount -l edit/lib/modules/2.6.27-14-generic/volatile
umount squashfs
umount mnt
umount edit
echo "Done. Please run finalizedvd.sh $SVNrevision"
~/finalizedvd.sh $SVNrevision
mv ~/live/L*.iso /var/www/rsync
exit 0