#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Output=VGA
Resolution=1024x768

if [[ -f /tmp/avwizard-resolution-defaults.txt ]]; then
	DefaultSettings=$(</tmp/avwizard-resolution-defaults.txt)
	Output=$(SpcField 5 "$DefaultSettings")
fi

ParseParms()
{
	local i
	for ((i = 1; i <= "$#"; i++)); do
		case "${!i}" in
			--resolution) ((i++)); Resolution="${!i}" ;;
			--output) ((i++)); Output="${!i}" ;;
			--tvstandard) ((i++)); TVStandard="${!i}" ;;
		esac
	done
}

ParseParms "$@"
/usr/pluto/bin/AVWizard_UpdateResolution.sh set_resolution "$Resolution" 60 "$Output" "$TVStandard"
cp /tmp/avwizard-resolution.txt /tmp/avwizard-resolution-defaults.txt
