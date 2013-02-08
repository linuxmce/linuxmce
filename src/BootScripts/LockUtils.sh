#!/bin/bash

if [[ -n "$HEADER_LockUtils" ]]; then
	return 0
fi
HEADER_LockUtils=included

# TODO: real locks, with PID and locking host inside to detect stale locks

# Note: when a process is killed, these locks remain active
#       can't trap EXIT safely, so the above TODO will have to be implemented instead

Dir="/usr/pluto/locks"
mkdir -p "$Dir"

Lock()
{
	local Lock="$1"
	local Device="$2"
	local NoLog="$3"
	
	[[ -z "$NoLog" ]] && echo "$$ $(date) Lock '$Lock' ($Device)"

	[[ -z "$Lock" ]] && return 1
	
	if ln -s "/proc/$$-$Device" "$Dir/$Lock" 2>/dev/null; then
		[[ -z "$NoLog" ]] && echo "$$ $(date) Lock '$Lock' ($Device) success"
		return 0
	else
		[[ -z "$NoLog" ]] && echo "$$ $(date) Lock '$Lock' ($Device) fail"
		local Link=$(readlink "$Dir/$Lock") # /proc/PID-extra
		local Target=${Link%%-*} # /proc/PID
		if [[ -n "$Target" && ! -d "$Target" ]]; then
			echo "$$ $(date) Lock '$Lock' ($Device) stale. Removing $Dir/$Lock ($Link)"
			rm -f "$Dir/$Lock"
			echo "$$ $(date) Retrying lock '$Lock' ($Device)"
			Lock "$Lock" "$Device" "$NoLog"
			return $?
		fi
		return 1
	fi
}

# there is no ability to check if this our lock yet
Unlock()
{
	local Lock="$1"
	local Device="$2"
	local NoLog="$3"
	
	[[ -z "$NoLog" ]] && echo "$$ $(date) Unlock '$Lock' ($Device)"

	[[ -z "$Lock" ]] && echo "$$ $(date) Unlock '$Lock' ($Device) fail" && return 1
	rm -f "$Dir/$Lock" 2>/dev/null
	[[ -z "$NoLog" ]] && echo "$$ $(date) Unlock '$Lock' ($Device) success"
	return 0
}

TryLock()
{
	local Lock="$1"
	local Device="$2"
	local NoLog="$3"

	[[ -z "$NoLog" ]] && echo "$$ $(date) TryLock '$Lock' ($Device)"
	if Lock "$Lock" "$Device"; then
		[[ -z "$NoLog" ]] && echo "$$ $(date) TryLock '$Lock' ($Device) success"
		return 0
	else
		[[ -z "$NoLog" ]] && echo "$$ $(date) TryLock '$Lock' ($Device) fail"
		return 1
	fi
}

WaitLock()
{
	local Lock="$1"
	local Device="$2"
	local NoLog="$3"
	
	[[ -z "$NoLog" ]] && echo "$$ $(date) WaitLock '$Lock' ($Device)"
	until Lock "$1" "$Device" nolog; do
		sleep 1
	done
	[[ -z "$NoLog" ]] && echo "$$ $(date) WaitLock '$Lock' ($Device) success"
	return 0
}
