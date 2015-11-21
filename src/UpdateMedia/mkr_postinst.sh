#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/SQL_Ops.sh



# One time also update the thumbnails and search tokens
# /usr/pluto/bin/UpdateMedia -t -s -d /home/public/data || :

#update-rc.d update-media stop 10 6 0 .
exit 0

#
# We don't need to run UpdateMedia at this point in time, as it is
# run on the next reboot anyhow.
#

Q="SELECT PK_Users FROM Users"
R="$(RunSQL "$Q")"

for User in $R; do
	Dir="/home/user_$User/data/documents"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir" || :
	fi
	Dir="/home/user_$User/data/audio"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir" || :
	fi
	Dir="/home/user_$User/data/pictures"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir" || :
	fi
	Dir="/home/user_$User/data/videos"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir" || :
	fi
	Dir="/home/user_$User/data/games/MAME"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir" || :
	fi
	Dir="/home/user_$User/data/games/a2600"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir" || :
	fi
	Dir="/home/user_$User/data/games/a2600p"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir" || :
	fi
done

## Add the init script to runlevel 0 and 6
