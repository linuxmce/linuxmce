#!/bin/bash
# finalizedvd.sh
# Wraps up the DVD builder process
# Copyright 2009, Peer Oliver Schmidt
# License GPL v3
#
SVNrevision=$1
echo Revision $SVNrevision
pushd ~/live
echo "Make files writeable"
chmod +w extract-cd/casper/filesystem.manifest
echo "Create a new manifest file"
#
# Create the manifest file, containng a list of installed applications
#
sudo chroot edit dpkg-query -W --showformat='${Package} ${Version}\n' > extract-cd/casper/filesystem.manifest
sudo cp extract-cd/casper/filesystem.manifest extract-cd/casper/filesystem.manifest-desktop
sudo sed -i '/ubiquity/d' extract-cd/casper/filesystem.manifest-desktop

#
# The installed system is now compressed. This compressed system is later de-compressed and copied over to the new systemn
#
echo Compress filesystem
sudo rm extract-cd/casper/filesystem.squashfs
sudo mksquashfs edit extract-cd/casper/filesystem.squashfs -nolzma -processors 2 -e edit/proc/* edit/lib/modules/2.6.27-14-generic/volatile 
echo Done mksquashfs - PRESS A KEY
echo Set a name to the disk image
IMAGE_NAME="LinuxMCE 8.10 i386"

echo Set name of the disk
sudo sed -i '/#define DISKNAME/d' extract-cd/REAMDE.diskdefines
echo "#define DISKNAME  $IMAGE_NAME">>extract-cd/README.diskdefines

#echo Create pre-installer
#cat <<eol > extract-cd/laterun.sh
##!/bin/bash
#cp -r /usr/pluto /target/usr 
#touch /target/ichwarda
#eol
#chmod +x extract-cd/laterun.sh

echo "Creating preseed file for LinuxMCE install"
cat <<eol > extract-cd/preseed/lmce.seed
# Install the Kubuntu desktop.
tasksel	tasksel/first	multiselect kubuntu-desktop
# Install KDE translation packages.
d-i	pkgsel/language-pack-patterns	string language-pack-kde-$LL kde-i18n-$LL
# Install the KDE oem-config frontend (if in OEM mode).
d-i	oem-config-udeb/frontend	string kde
d-i	debian-installer/locale		string en_US
d-i	debian-installer/layoutcode	string us
d-i	time/zone	string Europe/Berlin
d-i	partman-auto/disk	string	regular
d-i	partman-auto/choose_recipe	select atomic
d-i	partman/confirm_write_new_label	boolean	true
d-i	partman/choose_partition	select	finish
d-i	partman/confirm boolean	true
d-i	apt-setup/restricted	boolean	true
d-i	apt-setup/universe	boolean	true
d-i	debian-installer/allow_unauthenticated	string	true
d-i	pkgsel/include	string mysql-server openssh-server 
d-i	apt-setup/local0/repository	string deb file:/usr/pluto/deb-cache ./
d-i	apt-setup/local1/repository	string deb http://www.avenard.org/files/ubuntu-repos intrepid release
d-i	apt-setup/local2/repository	string deb http://deb.linuxmce.org/ intrepid beta2
# d-i	preseed/late_command	string	bash /laterun.sh
d-i	ubiquity/success_command	string	cp -r /cdrom/usr/pluto /target/usr
d-i	ubiquity/summary	note
d-i	ubiquity/reboot	boolean	true
#apt-install zsh; in-target chsh -s /bin/zsh
eol

echo "Adding LinuxMCE install selection to boot menu" 
echo "
label LinuxMCE
  menu label ^LinuxMCE Install
  kernel /casper/vmlinuz
  append  file=/cdrom/preseed/lmce.seed boot=casper only-ubiquity initrd=/casper/initrd.gz quiet splash --
">> extract-cd/isolinux/text.cfg
sed -i s/default live/default LinuxMCE/g extract-cd/isolinux/text.cfg
echo Calculate a new MD5sum
sudo rm extract-cd/md5sum.txt
sudo find extract-cd -type f -print0 | xargs -0 md5sum > extract-cd/md5sum.draft
sudo cat extract-cd/md5sum.draft | sudo grep -v md5sum.txt | sudo grep -v isolinux/boot.cat > extract-cd/md5sum.txt
rm extract-cd/md5sum.draft

echo Create ISO Image
cd extract-cd
ISONAME=../LinuxMCE-8.10-$SVNrevision-i386.iso
SVNrevision=$(svn info "$svn_dir/$svn_branch_name/src" |grep ^Revision | cut -d" " -f2)
sudo mkisofs -allow-limited-size -quiet -D -r -V "$IMAGE_NAME" -cache-inodes -J -l -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -o $ISONAME .
echo Done!
exit 0