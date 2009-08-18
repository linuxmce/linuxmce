#!/bin/bash

## This script sends a string to a network device and read a reply
## Then it looks to see if the test string is a substring of the reply
## Return: 0 (OK, substring match), 1 (Not OK, no match)
## Depends on: shell-io

## Functionality is similar to TestSerialPort, but for network devices instead of serial ports,
## and it can be a bit cryptic, yet powerful

## Something that may be confusing:
## "Encryption":
## Raw: whatever you pass as SendString is forwarded as-is; TestString is raw as well
## Enc: the SendString and TestString are coded with UrlEncode
## Hex: the SendString and TestString are coded in full hex

## ****************** ##
## Script starts here ##
## ****************** ##

## Parse the parameters
## Usage: ParseParameters "$@"
## Returns: OR Bitfield (see ValidateParameters)
ParseParameters()
{
	local i
	local ParmValue
	local Var

	## Make sure no Parm_* variables are set
	for Var in ${!Parm_*}; do
		eval "unset $Var"
	done

	## If the parameter list is empty, just display help
	if [[ "$#" -eq 0 ]]; then
		Parm_ShowHelp="ShowHelp"
	fi

	## Defaults
	Parm_Encoding=raw

	for ((i = 1; i <= $#; i++)); do
		ParmValue="${!i}"
		case "${ParmValue}" in
			--ip) ((i++)); Parm_IPAddress="${!i}" ;;
			--port) ((i++)); Parm_Port="${!i}" ;;
			--protocol) ((i++)); Parm_Protocol="${!i}" ;;
			--timeout) ((i++)); Parm_Timeout="${!i}" ;; # in seconds
			--send-string) ((i++)); Parm_SendString="${!i}" ;;
			--test-string) ((i++)); Parm_TestString="${!i}" ;;
			--encoding) ((i++)); Parm_Encoding="${!i}" ;;
			--help) Parm_ShowHelp="ShowHelp" ;;
		esac
	done

	ValidateParameters
	return $?
}

## Bitfield values used by ValidateParameters
## AKA ValidateParameterers eXitcode
VPX_OK=0
VPX_BadIP=1
VPX_BadPort=$((1<<1))
VPX_BadProtocol=$((1<<2))
VPX_BadTimeout=$((1<<3))
VPX_EmptySendString=$((1<<4))
VPX_BadEncoding=$((1<<5))

## Validate parameters
## Called by: ParseParameters
## Usage: ParseParameters
## Returns: OR Bitfield (forwarded by ParseParameters)
ValidateParameters()
{
	local VPX=0
	if [[ -z "$Parm_IPAddress" || ! "$Parm_IPAddress" =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
		# Note: numbers between 255-999 will pass as valid
		((VPX |= VPX_BadIP))
	fi
	if [[ -z "$Parm_Port" || "$Parm_Port" =~ [^0-9] ]]; then
		((VPX |= VPX_BadPort))
	fi
	if [[ -z "$Parm_Protocol" || ! "$Parm_Protocol" =~ ^(tcp|udp)$ ]]; then
		((VPX |= VPX_BadProtocol))
	fi
	if [[ -z "$Parm_Timeout" || ! "$Parm_Timeout" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
		((VPX |= VPX_BadTimeout))
	fi
	if [[ -z "$Parm_SendString" ]]; then
		((VPX |= VPX_EmptySendString))
	fi
	## Parm_TestString can be empty
	if [[ ! "$Parm_Encoding" =~ ^(raw|enc|hex)$ ]]; then
		((VPX |= VPX_BadEncoding))
	fi

	return "$VPX"
}

## Convert VPX bitfield to human-readable string
## Usage: PrintVPX "$VPX_BitField"
## Returns: 0
PrintVPX()
{
	local VPX="$1"
	if [[ "$VPX" =~ [^0-9] ]]; then
		return 0
	fi

	if [[ "$VPX" -eq 0 ]]; then
		echo "OK"
	fi
	if (( (VPX & VPX_BadIP) != 0 )); then
		echo "Bad IP address"
	fi
	if (( (VPX & VPX_BadPort) != 0 )); then
		echo "Bad port value"
	fi
	if (( (VPX & VPX_BadProtocol) != 0 )); then
		echo "Bad protocol value"
	fi
	if (( (VPX & VPX_BadTimeout) != 0 )); then
		echo "Bad timeout value"
	fi
	if (( (VPX & VPX_EmptySendString) != 0 )); then
		echo "Empty string to send"
	fi
	if (( (VPX & VPX_BadEncoding) != 0 )); then
		echo "Bad encoding"
	fi

	return 0
}

## Displays command usage instructions
## Usage: PrintHelp
PrintHelp()
{
	printf "%s\n" "Usage:"
	printf "\t%s\n" "$0 --protocol {tcp|udp} --ip <IP Address> --port <Port>"
	printf "\t\t%s\n" "--timeout <Seconds> [--encoding {raw|enc|hex}]"
	printf "\t\t%s\n" "--send-string <SendString> [--test-string <TestString>]"
	printf "\t%s\n" "$0 --help"

	return 0
}

ParseParameters "$@"
Error=$?

if [[ "$Parm_ShowHelp" == "ShowHelp" ]]; then
	PrintHelp >&2
	exit 0
fi

if [[ "$Error" != 0 ]]; then
	PrintVPX "$Error" >&2
	PrintHelp >&2
	exit 1
fi

## Open file descriptor 3 to communicate with a network device
## Usage: ConnectToRemoteHost {tcp|udp} <IP_Address> <Port>
ConnectToRemoteHost()
{
	local Protocol="$1" IP="$2" Port="$3"
	local SocatProtocol
	local PtyFile
	PID_RemoteHostSocat= # global

	case "$Protocol" in
		tcp) SocatProtocol=TCP4 ;;
		udp) SocatProtocol=UDP4 ;;
		*) return 1 ;;
	esac

	PtyFile="/tmp/ProbeNetDevice.$$.netdev_socket"
	socat PTY,link="$PtyFile",echo=0,icanon=0,raw "$SocatProtocol:$IP:$Port" &
	PID_RemoteHostSocat=$!
	sleep .5
	exec 3<>"$PtyFile"
	rm -f "$PtyFile"
}

## Disconnect from the remote host if connected
## Usage: DisconnectFromRemoteHost
DisconnectFromRemoteHost()
{
	if [[ -e /proc/$$/fd/3 ]]; then
		exec 3>&-
	fi

	if [[ -n "$PID_RemoteHostSocat" && -d "/proc/$PID_RemoteHostSocat" ]] && grep -qF "$0" "/proc/$PID_RemoteHostSocat/cmdline"; then
		kill "$PID_RemoteHostSocat"
	fi
}

## Effectively probe the device
## Usage: ProbeDevice <Timeout> <SendString> [<TestString>]
## Returns: 0 (Match), 1 (Not match), 2 (Internal error)
ProbeDevice()
{
	local Encoding="$1" Timeout="$2" SendString="$3" TestString="$4"
	local FDReady
	local ReturnString

	if [[ ! -e /proc/$$/fd/3 ]]; then
		return 2
	fi

	/usr/pluto/bin/fdwrite "--$Encoding" --fd 3 --data "$SendString"

	sleep "$Timeout"
	FDReady=$(/usr/pluto/bin/fdselect 0 0 3)
	ReturnString=
	if [[ -n "$FDReady" ]]; then # it's always either 3 or none, in this case
		ReturnString=$(/usr/pluto/bin/fdread "--$Encoding" --stream --count 4096 --fd 3)
	fi

	echo -n "$ReturnString" | od -tx1c

	if [[ -z "$TestString" ]]; then
		## Empty test string always matches
		return 0
	elif [[ "$ReturnString" =~ "$TestString" ]]; then
		## the return string contains the test string
		return 0
	else
		## no match
		return 1
	fi
}

ConnectToRemoteHost "$Parm_Protocol" "$Parm_IPAddress" "$Parm_Port"
ProbeDevice "$Parm_Encoding" "$Parm_Timeout" "$Parm_SendString" "$Parm_TestString"
ProbeResult=$?
DisconnectFromRemoteHost

exit "$ProbeResult"
