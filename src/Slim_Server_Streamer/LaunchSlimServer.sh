#!/bin/bash

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
	-d) ((i++)); DeviceID="${Params[$i]}" ;;
        esac
done
                                

PORT="9090"
PARAMS="--cliport $PORT --audiodir /home/public/data/audio"
sed -ri "s,SLIMOPTIONS=.*,SLIMOPTIONS=\"$PARAMS\",g" /etc/default/squeezeboxserver

found=0

# Three settings need to exist in the server.prefs file.
grep "audiodir: /home/public/data/audio" /var/lib/squeezeboxserver/prefs/server.prefs && let found+=1
grep "playlistdir: /home/public/data/audio" /var/lib/squeezeboxserver/prefs/server.prefs && let found+=1
grep "wizardDone: 1" /var/lib/squeezeboxserver/prefs/server.prefs && let found+=1
if [ "$found" -ne "3"] ; then
        echo "audiodir: /home/public/data/audio" >> /var/lib/squeezeboxserver/prefs/server.prefs
        echo "playlistdir: /home/public/data/audio" >> /var/lib/squeezeboxserver/pres/server.prefs
        echo "wizardDone: 1" >> /var/lib/squeezeboxserver/prefs/server.prefs
	invoke-rc.d squeezeboxserver restart
fi                                

./Slim_Server_Streamer -d $DeviceID
