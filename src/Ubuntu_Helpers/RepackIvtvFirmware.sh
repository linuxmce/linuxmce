#!/bin/bash

rm -f ivtv-firmware_*.deb
aptitude download ivtv-firmware
mkdir ivtv-firmware
cd ivtv-firmware
dpkg -x ../ivtv-firmware_*.deb .
dpkg -e ../ivtv-firmware_*.deb
rm -f DEBIAN/preinst
rm -f ../ivtv-firmware_*.deb
dpkg-deb -b . ..
cd ..
rm -rf ivtv-firmware
