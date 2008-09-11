#!/bin/bash

# Parse parameters for files.d scripts

for ((i = 1; i <= "$#"; i++)); do
	Parm="${!i}"
	case "$Parm" in
		--root) ((i++)); Parm_RootLocation="${!i}" ;;
		--device) ((i++)); Parm_Device="${!i}" ;;
		--subname) ((i++)); Parm_Subname="${!i}" ;;
		--template) ((i++)); Parm_Template="${!i}" ;;
		*) echo "ERR: Unknown parameter '$Parm'"; exit 1 ;;
	esac
done

if [[ ! -d "$Parm_RootLocation" ]] ;then
	echo "ERROR: Directory "$Parm_RootLocation" does not exist"
	exit 1
fi

if [[ -z "$Parm_Device" ]]; then
	echo "ERROR: Device not specified"
	exit 1
fi
