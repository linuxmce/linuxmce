#!/bin/bash

function XorgConfLogging() {
        local message="$1"
        local xorgLog="/var/log/pluto/xorg.conf.log"
        local xorgLines=$(cat /etc/X11/xorg.conf | wc -l)
				
        local myPid=$$

        echo "$myPid $(date -R) $message [$xorgLines]"  >> $xorgLog
}

XorgConfLogging "Starting $0 $*"
trap 'XorgConfLogging "Ending"' EXIT


ConfigFile="/etc/X11/xorg.conf"

for ((i = 1; i <= $#; i++)); do
	case "${!i}" in
		--conffile) ((i++)); ConfigFile="${!i}" ;;
		--driver) ((i++)); Driver="${!i}" ;;
		*) echo "Unknown option '$1'"; exit 1 ;;
	esac
done

XModulesAdd=(dbe extmod record speedo type1 glx int10 v4l)
XModulesRemove=()

XModulesActive=($(awk -f/usr/pluto/bin/X-ModulesSection.awk "$ConfigFile"))

# if driver is 'nvidia', disable the 'dri' module
# nVidia implements its own DRI, and the X DRI module causes big problems with the nVidia drivers - vox populi
if lspci | grep -qi 'VGA .* nVidia)'; then
	XModulesRemove=("${XModulesRemove[@]}" dri)
else
	# all other cards will load the 'dri' module
	XModulesAdd=("${XModulesAdd[@]}" dri)
fi

# Remove modules from active list
for ((i = 0; i < "${#XModulesRemove[@]}"; i++)); do
	for ((j = 0; j < "${#XModulesActive[@]}"; j++)); do
		if [[ "${XModulesRemove[$i]}" == "${XModulesActive[$j]}" ]]; then
			unset "XModulesActive[$j]"
			break
		fi
	done
done
XModulesActive=("${XModulesActive[@]}")

# Add missing modules to active list
for ((i = 0; i < "${#XModulesAdd[@]}"; i++)); do
	Add=1
	for ((j = 0; j < "${#XModulesActive[@]}"; j++)); do
		if [[ "${XModulesAdd[$i]}" == "${XModulesActive[$j]}" ]]; then
			Add=0
			break
		fi
	done
	if [[ "$Add" == 1 ]]; then
		XModulesActive=("${XModulesActive[@]}" "${XModulesAdd[$i]}")
	fi
done

awk -v"ModList=${XModulesActive[*]}" -f/usr/pluto/bin/X-ModulesSection.awk "$ConfigFile" >"$ConfigFile".$$
mv "$ConfigFile"{.$$,}
