#!/bin/bash
USER=pos
if [[ "x$1" != "x" ]]; then
	USER=$1
fi
echo "Using $USER to connect to the repo"
scp /var/lmce-build/out/tmp/*.deb $USER@deb.linuxmce.org:/var/www/ubuntu/dists/intrepid/beta2/binary-i386
# home/$USER/newdebs/
# rsh $USER@deb.linuxmce.org sudo mv /home/$USER/newdebs/*.deb /var/www/ubuntu/dists/intrepid/lmce-alpha-latest-updates/binary-i386 -v
rsh $USER@deb.linuxmce.org sudo /home/$USER/update_latest.sh
