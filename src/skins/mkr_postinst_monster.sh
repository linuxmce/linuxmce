#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

PrevVer="$2"

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

ConfSet UseVideoWizard 1
ConfSet ReplaceSkin_1 7
ConfSet ReplaceSkin_3 10
ConfSet ReplaceSkin_6 11
ConfSet UseSkinSettings_7 1
ConfSet UseSkinSettings_10 3
ConfSet UseSkinSettings_11 6

mv /var/www/lmce-admin/include/images/{monster_logo,logo_pluto}.jpg || /bin/true
cp /usr/pluto/orbiter/skins/Monster/bootsplash/*.cfg /etc/bootsplash/themes/pluto/config || /bin/true
cp /usr/pluto/orbiter/skins/Monster/bootsplash/*.jpg /etc/bootsplash/themes/pluto/images || /bin/true
/var/lib/dpkg/info/bootsplash.postinst configure

