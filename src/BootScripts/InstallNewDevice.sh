#!/bin/bash

# Re-run ourselves with stdout and stderr piped through tee
. /usr/pluto/bin/TeeMyOutput.sh --outfile /var/log/pluto/InstallNewDevice.log --infile /dev/null --stdboth --append -- "$@"

. /usr/pluto/bin/LockUtils.sh

echo -n "$(date -R) InstallNewDevice: Called with parameters:"
for Parm in "$@"; do
	echo -n " $Parm"
done
echo

Device="$1"
Package="$2"

TryInstall()
{
	WaitLock "InstallNewDevice" "$Device"
	trap "Unlock 'InstallNewDevice' '$Device'" EXIT

	error=1
	retries=0
	while [[ $retries < 9 && $error != 0 ]]; do
		apt-get -y install "$Package"
		error=$?
		if [[ $error != 0 ]]; then
			echo "Try $retries failed with exit code $error"
			sleep 5
		fi
		((retries++))
	done

	if [[ $retries == 9 ]]; then
		echo "All retries failed"
	else
		echo "Package installed successfully"
	fi
}

if [[ -z "$Device" || -z "$Package" ]]; then
	echo "Bad parameters: '$Device', '$Package'"
	exit 1
fi

TryInstall
disown -a
