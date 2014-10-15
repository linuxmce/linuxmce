#!/bin/bash

if [[ -n "$HEADER_Config_Ops" ]]; then
	return 0
fi
HEADER_Config_Ops=included

NoSpace="s/ //g"
LogSectionDelimiter="========== NEW LOG SECTION =========="
ConfForbiddenNameChars="=\\"
ConfForbiddenValueChars=""

. /usr/pluto/bin/LockUtils.sh

ConfLogError()
{
	local Value="$1" WhatItIs="$2"
	{
		builtin echo "$LogSectionDelimiter"
		date -R
		builtin echo "Value: $Value"
		builtin echo "Type: $WhatItIs"
		ls -l /usr/pluto/locks/
		ps axf
	} >>/var/log/ConfError.log
}

ConfValidValue()
{
#	local Value="$1" WhatItIs="$2" Forbidden="$3"
#	local i j
#	local ValueChar ForbiddenChar
#
#	for ((i = 0; i < "${#Value}"; i++)); do
#		ValueChar="${Value:$i:1}"
#		if [[ "$ValueChar" < " " || "$ValueChar" > "~" ]]; then
#				ConfLogError "$Value" "$WhatItIs"
#				return 1
#		fi
#		for ((j = 0; j < "${#Forbidden}"; j++)); do
#			ForbiddenChar="${Forbidden:$j:1}"
#			if [[ "$ValueChar" == "$ForbiddenChar" ]]; then
#				ConfLogError "$Value" "$WhatItIs"
#				return 1
#			fi
#		done
#	done
#
	return 0
}

ConfEval()
{
	local Ret=0
	WaitLock "pluto.conf" "Config_Ops-ConfEval" nolog

	# make sure Display is unset and we don't get it from the environment
	unset Display

	if [[ ! -e /etc/pluto.conf ]]; then
		touch /etc/pluto.conf
	fi

	# Note to self: this "read from file into while" is absolutely necessary
	#               "cmd | while ..." spawns a subshell and our veriables will be set there instead of our current shell
	#               "while ...; do ...; done <file" works the way we need
	while read line; do
		line="${line// }"
		if [[ "$line" == "#"* || "$line" == "//"* || "$line" == ";"* ]]; then
			continue # comment line; skip
		fi
		eval "export $line" &>/dev/null
	done </etc/pluto.conf
	Unlock "pluto.conf" "Config_Ops-ConfEval" nolog

	# if there was no MySqlDBName in pluto.conf, set it to pluto_main
	if [[ -z "$MySqlDBName" ]]; then
		ConfSet MySqlDBName pluto_main
	fi

	# if there was no Display in pluto.conf, set it to 0
	# rationale: Installing 2.0.0.44 by using a 2.0.0.43 CD needs this, otherwise X won't start
	if [[ -z "$Display" ]]; then
		ConfSet Display 0
	fi
	return $Ret
}

ConfSet()
{
	local Ret=0
	WaitLock "pluto.conf" "Config_Ops-ConfSet" nolog
	local Variable="$1" Value="$2"
	local EscVariable="${Variable//\\\\/\\\\}" EscValue="${Value//\\\\/\\\\}"

	if ! ConfValidValue "$Variable" "Variable" "$ConfForbiddenNameChars" || ! ConfValidValue "$Value" "Value" "$ConfForbiddenValueChars"; then
		Unlock "pluto.conf" "Config_Ops-ConfSet" nolog
		return 1
	fi

	local Line="$Variable = $Value"
	local EscLine="${Line//\\\\/\\\\}"
	if ! grep "^$EscVariable.*=" /etc/pluto.conf &>/dev/null; then
		builtin echo "$Line" >> /etc/pluto.conf
	else
		sed -i "s/^$EscVariable.*=.*$/$EscLine/g" /etc/pluto.conf
	fi
	eval "export $EscVariable=\"$EscValue\"" &>/dev/null
	Unlock "pluto.conf" "Config_Ops-ConfSet" nolog
	return $Ret
}

ConfGet()
{
	local Ret=0
	WaitLock "pluto.conf" "Config_Ops-ConfGet" nolog
	local Variable="$1" Line
	if ! grep "^$Variable.*=" /etc/pluto.conf &>/dev/null; then
		Ret=1
	else
		Line=$(grep "^$Variable.*=" /etc/pluto.conf 2>/dev/null | head -1 | sed "$NoSpace")
		eval "export $Line"
	fi
	Unlock "pluto.conf" "Config_Ops-ConfGet" nolog
	return $Ret
}

ConfDel()
{
	local Variable="$1"
	local EscVariable="${Variable//\\\\/\\\\}"
	WaitLock "pluto.conf" "Config_Ops-ConfDel" nolog
	sed -i "/^$EscVariable.*=/ d" /etc/pluto.conf
	eval "unset $Variable"
	Unlock "pluto.conf" "Config_Ops-ConfDel" nolog
}

ReplaceVars()
{
	# TODO
	local File Commands VarValue SedCmd
	File="$1"
	
	for i in $Vars; do
		eval "VarValue=\"\$$i\""
		VarValue=${VarValue//\//\\\/}
		VarValue=$(builtin echo "$VarValue" | sed 's/^ *//g; s/ *$//g')
		SedCmd="s/%$i%/$VarValue/g"
		[[ -z "$Commands" ]] && Commands="$SedCmd" || Commands="$Commands; s/%$i%/$VarValue/g"
	done
	sed -i "$Commands" $File
}

PackageIsInstalled()
{
	local Pkg="$1"
	local RootFs="${2:-/}"

	[[ -z "$Pkg" ]] && return 1
	#dpkg -s "$Pkg" 2>/dev/null | grep -q 'Status: install ok installed'
	PackageStatus "$Pkg" "$RootFs" | grep -q '^Status: install ok installed'
}

PackageStatus()
{
	local Pkg="$1"
	local RootFs="${2:-/}"

	[[ -z "$Pkg" ]] && return 1
	dpkg --root="$RootFs" -s "$Pkg" 2>/dev/null || return 1
	return 0
}

InstalledPackages()
{
	local Pkg InstPkg=""
	local PkgList

	# Expand package list
	PkgList=()
	for Pkg in "$@"; do
		PkgList=("${PkgList[@]}" $(PackageStatus "$Pkg" | grep '^Package: ' | sed 's/^Package: //') )
	done

	for Pkg in "${PkgList[@]}"; do
		if PackageIsInstalled "$Pkg"; then
			InstPkg="$InstPkg $(PackageStatus $Pkg | grep '^Package: ' | sed 's/^Package: //')"
		fi
	done
	builtin echo "${InstPkg# }"
}

ConfEval

PLUTO_DB_CRED=""
if [[ -n "$MySqlHost" ]]; then PLUTO_DB_CRED="$PLUTO_DB_CRED -h $MySqlHost"; fi
if [[ -n "$MySqlPort" ]]; then PLUTO_DB_CRED="$PLUTO_DB_CRED -P $MySqlPort"; fi
if [[ -n "$MySqlUser" ]]; then PLUTO_DB_CRED="$PLUTO_DB_CRED -u $MySqlUser"; fi
if [[ -n "$MySqlPassword" ]]; then PLUTO_DB_CRED="$PLUTO_DB_CRED -p $MySqlPassword"; fi
PLUTO_DB_CRED=$(builtin echo $PLUTO_DB_CRED)
export PLUTO_DB_CRED

MYSQL_DB_CRED=""
if [[ -n "$MySqlHost" ]]; then MYSQL_DB_CRED="$MYSQL_DB_CRED -h$MySqlHost"; fi
if [[ -n "$MySqlPort" ]]; then MYSQL_DB_CRED="$MYSQL_DB_CRED -P$MySqlPort"; fi
if [[ -n "$MySqlUser" ]]; then MYSQL_DB_CRED="$MYSQL_DB_CRED -u$MySqlUser"; fi
if [[ -n "$MySqlPassword" ]]; then MYSQL_DB_CRED="$MYSQL_DB_CRED -p$MySqlPassword"; fi
# Make sure to trim excess spaces 
MYSQL_DB_CRED=$(builtin echo $MYSQL_DB_CRED)
export MYSQL_DB_CRED

LMCE_DIRS="pictures audio documents videos games/MAME games/a2600 games/a2600p games/a5200 games/a5200p games/a7800 games/a7800p games/coleco games/intv games/sg1000 games/sms games/smspal games/nes games/famicom games/snes games/snespal games/genesis games/megadriv games/vectrex games/tg16 games/pce games/sgx games/c64 games/apple2e games/ti99 pvr"
export LMCE_DIRS

VGcmd=""
if [[ -n "$Valgrind" ]]; then
	if which valgrind &>/dev/null; then
		VGcmd="valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=15 " # trailing space is mandatory
		export GLIBCXX_FORCE_NEW=1
		export GLIBCPP_FORCE_NEW=1
	else
		builtin echo "*** WARNING *** Valgrind not installed. Running programs the normal way ***"
	fi
fi

[[ -f /tmp/pluto.gdb ]] || builtin echo "run" >/tmp/pluto.gdb
if [[ -n "$GDB" ]]; then
	if which gdb &>/dev/null; then
		VGcmd="gdb --command=/tmp/pluto.gdb --args "
	elif [[ -z "$VGcmd" ]]; then
		builtin echo "*** WARNING *** GDB not installed. Running programs the normal way ***"
	fi
fi

ulimit -c unlimited
ulimit -n 4096
shopt -s nullglob
export HOME=/root
