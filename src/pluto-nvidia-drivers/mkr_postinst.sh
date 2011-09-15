#!/bin/bash

source /usr/pluto/bin/nvidia-install.sh
. /usr/pluto/bin/Utils.sh

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

if [ ! -f /etc/X11 ]; then
	mkdir -p /etc/X11
fi

# install the correct nvidia driver
installCorrectNvidiaDriver

#Reconfigure X with the updated video driver
bash /usr/pluto/bin/Xconfigure.sh --update-video-driver --keep-resolution | tee-pluto /var/log/pluto/Xconfigure.log
