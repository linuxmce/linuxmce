#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
XDisplay=":$Display"
((Xvt = Display + 7))

function DisplayUsage() {
	echo "Usage: RestartX.sh <orbiterID> <computerIP>"
}

if [[ "$#" != "2" ]] ;then
	DisplayUsage
	exit 1	
fi

orbiterID=$1
computerIP=$2

/usr/pluto/bin/LaunchRemoteCmd.sh "$computerIP" "
#<-mkr_b_via_b->
exit
#<-mkr_b_via_e->
	. /usr/pluto/bin/Utils.sh;

	bash -x /usr/pluto/bin/Xconfigure.sh --keep-resolution | tee-pluto /var/log/pluto/Xconfigure.log
	pidOfX=\$(ps ax|grep \"X $XDisplay -ignoreABI -ac -allowMouseOpenFail vt$Xvt\"|egrep -v 'grep|SCREEN'|awk '{print \$1}')
	ReloadDevicesOnThisMachine
	kill \$pidOfX
	sleep 2
	kill -9 \$pidOfX
	sleep 1
	/usr/pluto/bin/Start_X.sh
" &
