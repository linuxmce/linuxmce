#!/bin/bash

if [[ -n "$HEADER_MR_Conf" ]]; then
	return 0
fi
HEADER_MR_Conf=included

MR_ConfEval()
{
	local Ret=0
	local Flavor="${1:-pluto}"

	if [[ ! -f "/etc/MakeRelease/$Flavor.conf" ]]; then
		echo "ERROR: /etc/MakeRelease/$Flavor.conf doesn not exist"
		exit 1
	fi
	
	# Note to self: this "read from file into while" is absolutely necessary
	#               "cmd | while ..." spawns a subshell and our veriables will be set there instead of our current shell
	#               "while ...; do ...; done <file" works the way we need
	while read line; do
		if [[ "$line" == "#"* || "$line" == "//"* || -z "$line" ]]; then
			continue # comment line; skip
		fi
		var="${line%%=*}"
		value="${line#*=}"
		eval "export $var=\"$value\"" &>/dev/null
	done <"/etc/MakeRelease/$Flavor.conf"
	return $Ret
}
