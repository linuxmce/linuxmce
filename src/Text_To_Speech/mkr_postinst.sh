#!/bin/bash

#Check to see if the voices already exist...
if [ -d "/usr/share/festival/voices/us" ] ; then
	echo "Nitech HTS festival voices already installed, no action required."
	exit 0
fi

echo "Downloading and installing the Nitech HTS festival voices..."
cd /tmp/
mkdir -p hts_tmp
cd hts_tmp/
#Download the voices
wget -c http://download.linuxmce.org/Festival/festvox_nitech_us_awb_arctic_hts-2.1.tar.bz2
wget -c http://download.linuxmce.org/Festival/festvox_nitech_us_bdl_arctic_hts-2.1.tar.bz2
wget -c http://download.linuxmce.org/Festival/festvox_nitech_us_clb_arctic_hts-2.1.tar.bz2
wget -c http://download.linuxmce.org/Festival/festvox_nitech_us_rms_arctic_hts-2.1.tar.bz2
wget -c http://download.linuxmce.org/Festival/festvox_nitech_us_slt_arctic_hts-2.1.tar.bz2
wget -c http://download.linuxmce.org/Festival/festvox_nitech_us_jmk_arctic_hts-2.1.tar.bz2
wget -c http://download.linuxmce.org/Festival/cmu_us_kal_com_hts.tar.gz
wget -c http://download.linuxmce.org/Festival/cstr_us_ked_timit_hts.tar.gz

#Extract the voices
for t in $(ls); do 
	tar xvf $t
done

#Install the voices
sudo mkdir -p /usr/share/festival/voices/us
sudo mv lib/voices/us/* /usr/share/festival/voices/us
sudo mv lib/hts.scm /usr/share/festival/hts.scm

#Clean Up
cd /tmp/
rm -rf hts_tmp/

echo "Finished installing Nitech HTS voices."

sudo mkdir -p /var/lib/asterisk/sounds/pluto/
