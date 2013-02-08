#!/bin/bash

. /usr/pluto/bin/X-CleanupVideo.sh

LastVar=
for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--flags) LastVar=Parms_Flags;;
		--parms) LastVar=Parms_StartX ;;
		--client) LastVar=Parms_Client ;;
		--server) LastVar=Parms_Server ;;
		*)
			if [[ -z "$LastVar" ]]; then
				echo "ERROR: Bad parameter order"
				exit 1
			fi
			eval "$LastVar=(\${$LastVar[@]} \${!i})"
		;;
	esac
done

## If plymouth isn't stopped, DRM can't be initialized and KMS fails to be set up, preventing X from starting
stop plymouth

#Ensure KDM doesn't start on boot, ticket http://svn.linuxmce.org/trac.cgi/ticket/1342
echo '/bin/false'>/etc/X11/default-display-manager

timestampStart=$(date +%s)
#xinit /usr/pluto/bin/XWM_Wrapper.sh "$XClient" "${XClientParm[@]}" -- "$XDisplay" -ignoreABI -ac -allowMouseOpenFail "$VT" "${XServerParm[@]}"
xinit /usr/pluto/bin/XWM_Wrapper.sh "${Parms_Client[@]}" -- "${Parms_Server[@]}" -dpi 96
echo "Exit code: $?"
timestampEnd=$(date +%s)

## if "noreconf" flag is given, don't go through safety reconfiguration
if [[ " ${Parms_Flags[*]} " == *" noreconf "* ]]; then
	exit 0
fi

## This is vulnerable to unexpected time shifts (for example, if ntpdate runs in the meantime, it can set timestampEnd -before- timestampStart, giving a time travel effect)
echo "start: $timestampStart; end: $timestampEnd; diff: $((timestampEnd-timestampStart))"
if ((timestampEnd-timestampStart < 10)); then
	## if X exited too soon, reconfigure it, considering that the video card has changed and X needs to be reconfigured
	CleanupVideo
	sleep 1
	/usr/pluto/bin/Start_X.sh "${Parms_StartX[@]}"
fi
