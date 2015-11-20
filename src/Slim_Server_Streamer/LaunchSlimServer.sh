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
grep -q "audiodir: /home/public/data/audio" /var/lib/squeezeboxserver/prefs/server.prefs && let found+=1
grep -q "playlistdir: /home/public/data/audio" /var/lib/squeezeboxserver/prefs/server.prefs && let found+=1
grep -q "wizardDone: 1" /var/lib/squeezeboxserver/prefs/server.prefs && let found+=1
if [ "$found" -ne "3" ] ; then
	# According to niz the audiodir and playlist dir entries always exist
	sed -i 's|'"audiodir: ''"'|audiodir: /home/public/data/audio|' /var/lib/squeezeboxserver/prefs/server.prefs
	sed -i 's|'"playlistdir: ''"'|playlistdir: /home/public/data/audio|' /var/lib/squeezeboxserver/prefs/server.prefs
	echo 'wizardDone: 1' >> /var/lib/squeezeboxserver/prefs/server.prefs
	service squeezeboxserver restart
fi                                

./Slim_Server_Streamer -d $DeviceID
