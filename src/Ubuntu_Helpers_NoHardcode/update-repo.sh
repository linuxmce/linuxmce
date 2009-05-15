#!/bin/bash
USER=pos
scp /var/lmce-build/out/tmp/*.deb $USER@deb.linuxmce.org:/home/$USER/newdebs/
rsh $USER@deb.linuxmce.org sudo mv /home/$USER/newdebs/*.deb /var/www/ubuntu/dists/intrepid/lmce-alpha-latest-updates/binary-i386 -v
rsh $USER@deb.linuxmce.org sudo /home/$USER/update_latest.sh
