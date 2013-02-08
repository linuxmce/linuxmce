#!/bin/bash

if [[ -n "$HEADER_Diskless_Utils" ]]; then
	return 0
fi
HEADER_Diskless_Utils=included

. /usr/pluto/bin/pluto.func

DecideKernelVersion()
{
	local Architecture="${1:-686}"

	if [[ "$Architecture" != *-smp ]]; then
		Architecture_Alt="$Architecture-smp"
	else
		Architecture_Alt="${Architecture%-smp}"
	fi

	KERNEL_VERSION_NOW="$(uname -r)"
	KERNEL_VERSION="${KERNEL_VERSION_NOW%*-*86*}-$Architecture" # Our kernels always report the architecture they're compiled for last
	KERNEL_VERSION_ALT="${KERNEL_VERSION_NOW%*-*86*}-${Architecture_Alt}" # If initial choice fails, use this

	deb_version=$(dpkg -l linux-image-`uname -r` | grep '^ii' |cut -d ' ' -f4)
	kernel="$(find /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ -name "linux-image-${KERNEL_VERSION}_${deb_version}_*.deb")"	
	kernel_alt="$(find /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ -name "linux-image-${KERNEL_VERSION_ALT}_${deb_version}_*.deb")"	
	words="$(echo "$kernel" | wc -w)"
	words_alt="$(echo "$kernel_alt" | wc -w)"
	if [[ "$words" -eq 0 && "$words_alt" -eq 0 ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "No kernel matching 'linux-image-$KERNEL_VERSION' and 'linux-image-$KERNEL_VERSION_ALT' was found"
		echo
		echo
		return 1
	fi
	if [[ "$words" -gt 1 ]]; then
		kernel="$(echo "$kernel" | cut -d' ' -f1)"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "More than one kernel found (this shouldn't happen). Using '$kernel'."
	elif [[ "$words" -eq 0 ]]; then
		kernel="$(echo "$kernel_alt" | cut -d' ' -f1)"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Using alternative kernel '$kernel'"
		KERNEL_VERSION="$KERNEL_VERSION_ALT"
	fi
	echo "$kernel"
	echo "$KERNEL_VERSION"
}
