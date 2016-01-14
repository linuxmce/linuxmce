#!/bin/bash

## This file is a helper called by RegenOrbiterOnTheFly.sh

OrbiterDev="$1"

[[ -z "$OrbiterDev" ]] && return

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

export SDL_VIDEODEVICE=dummy

SkinDir=/usr/pluto/orbiter/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto/orbiter

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating Orbiter on the fly nr. $OrbiterDev"

Q="UPDATE Orbiter set RegenPercent=0 where PK_Orbiter=$OrbiterDev"
RunSQL "$Q"
/usr/pluto/bin/MessageSend "$DCERouter" 0 $OrbiterDev 7 1 163 "RegenOrbiterOnTheFly.sh"

echo "on the fly regen of $OrbiterDev $2 $3" >> /var/log/pluto/orbitergen.log
/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" $3 -g "$SkinDir" -f "$FontDir" -o "$OutDir" $PLUTO_DB_CRED -D "$MySqlDBName"  >> /var/log/pluto/orbiterg$

# Notify the Orbiter Plugin that we finished
/usr/pluto/bin/MessageSend "$DCERouter" 0 $2 1 267 2 $OrbiterDev

exit 0
