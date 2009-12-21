#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Utils.sh

trap 'Unlock "UM" "Update_Media"' EXIT
Lock "UM" "Update_Media"

dir_list="/home/public/data"
for dir in /home/user_*/;do 
	if [[ -d $dir/data ]] ;then 
		dir_list="${dir_list}||${dir}data" 
	fi
done

if VDRInstalled; then
	. /etc/default/vdr
	if [ "${VIDEO_DIR}" != "" ]; then
		dir_list="${dir_list}||${VIDEO_DIR}"
	fi
fi
                                        

counter=30
while ((counter > 0)); do
	nice -n 20 /usr/pluto/bin/UpdateMedia $PLUTO_DB_CRED -D "$MySqlDBName" -U "/mnt/upnp" -d "${dir_list}" -B -t | tee-pluto -a /var/log/pluto/UpdateMedia.log
	((counter--))
done


function on_exit {

	echo -n "Waiting for update media to shut down"
	while "$(pidof UpdateMedia)" != "" ;do
		kill UpdateMedia
		sleep 0.5
		echo -n "."
	fi
	echo 

	Unlock "UM" "Update_Media"
}

trap "on_exit" EXIT
