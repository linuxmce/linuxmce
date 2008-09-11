#!/bin/bash

rm -f ivtv-firmware_*.deb
aptitude download ivtv-firmware=0.20070217 > /dev/null
mkdir ivtv-firmware
cd ivtv-firmware
dpkg -x ../ivtv-firmware_*.deb . > /dev/null
dpkg -e ../ivtv-firmware_*.deb > /dev/null
rm -f DEBIAN/preinst
rm -f ../ivtv-firmware_*.deb
dpkg-deb -b . .. > /dev/null
cd ..
rm -rf ivtv-firmware
