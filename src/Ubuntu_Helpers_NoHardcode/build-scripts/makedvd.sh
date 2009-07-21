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

SourceIso=kubuntu-8.10-desktop-i386.iso
BUILDER_ROOT=/opt/intrepid-i386
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
#echo "Acquire::http::Proxy \"http://10.1.3.5:3128\";">edit/etc/apt/apt.conf
sudo mount -t proc none ~/live/edit/proc
echo "Get Installer from build environment"
cp -r $BUILDER_ROOT/var/lmce-build/svn/branches/LinuxMCE-0810/src/new-installer edit/root/

echo Now updating the install
echo Creating an upgrade script
cp ~/installed-programs edit/root/
cat <<eol >edit/root/upgrade.sh
#!/bin/bash
export LC_ALL=C
export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get dist-upgrade -y
dpkg --configure -a --abort-after 20000
apt-get autoremove -y
wget http://deb.linuxmce.org/ubuntu/kubuntu810desktop-selection
dpkg --set-selections < kubuntu810desktop-selection
apt-get dselect-upgrade -y
cd /root/new-installer
bash pre-install-from-DVD.sh
eol

chmod +x edit/root/upgrade.sh
sudo chroot edit root/upgrade.sh

echo "deb http://archive.ubuntu.com/ubuntu/ intrepid  main restricted universe multiverse">> edit/etc/apt/sources.list


echo Creating Desktop button
mkdir -p edit/etc/skel/Desktop
cat <<eol >edit/etc/skel/Desktop/LinuxMCE
[Desktop Entry]
Encoding=UTF-8
Version=8.10
Type=Application
Terminal=false
Exec=kdesudo /root/new-installer/mce-install.sh
Path=/root/new-installer
Name=LinuxMCE Installer
Icon=gnome-panel-launcher
Terminal=true
TerminalOptions=\s--noclose
eol

echo "Copying over the current debs"
mkdir -p extract-cd/usr/pluto/deb-cache
cp $BUILDER_ROOT/var/www/{*.deb,Package*,Release*} extract-cd/usr/pluto/deb-cache

umount -l edit/{proc,sys}
umount -l edit/lib/modules/2.6.27-14-generic/volatile
umount squashfs
umount mnt
umount edit
echo "Done. Please run finalizedvd.sh"
