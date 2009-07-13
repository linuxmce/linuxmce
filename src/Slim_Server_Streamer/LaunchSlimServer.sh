#!/bin/bash

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
	-d) ((i++)); DeviceID="${Params[$i]}" ;;
        esac
done
                                

PORT="9090"
PARAMS="--cliport $PORT --audiodir /home/public/data/audio"

sed -ri "s,SLIMOPTIONS=.*,SLIMOPTIONS=\"$PARAMS\",g" /etc/default/squeezecenter

invoke-rc.d squeezecenter restart

./Slim_Server_Streamer -d $DeviceID
