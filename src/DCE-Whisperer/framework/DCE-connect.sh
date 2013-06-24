#!/bin/bash

## DCE communication functions

# Description: Sends a message to the DCE router
# Usage: SendToDCE DevFrom DevTo MsgType MsgID [Parm1 Value1...]
# Caveat: Parameter values need to be encoded and without spaces
SendToDCE()
{
	local Cmd="$1"

	if [[ "$Framework_DelayDCE" == DontDelayDCE ]]; then
		Log "Sending DCE command: $Cmd"
		/usr/pluto/bin/fdwrite --raw --fd "$Framework_FD_DCE" --data "$Cmd"$'\n'
	else
		Log "Queuing DCE command: $Cmd"
		Framework_DCEqueue=("${Framework_DCEqueue[@]}" "$Cmd")
	fi
}

# Description: Used when a command function finishes, to reply to the framework
# Usage: ReplyToDCE ReplyStr ReplyCmd
# Caveat: ReplyCmd is constructed in exactly the same way as the command passed
#         to SendToDCE above
ReplyToDCE()
{
	local String0="$1" OutVars="$2"
	builtin printf "%s\n%s\n" "$(ParmEncode "$String0")" "$OutVars"
}

# Description: Ask the router if a device has registered
# Usage: DeviceIsRegistered DeviceNumber
DeviceIsRegistered()
{
	local DeviceNumber="$1"

	/usr/pluto/bin/fdwrite --raw --fd "$Framework_FD_DCE" --data "DEVICE_REGISTERED $1"$'\n'
	local Reply=$(/usr/pluto/bin/fdread --raw --line --eol $'\n' --fd "$Framework_FD_DCE")

	if [[ "$Reply" == "DEVICE_REGISTERED "* ]]; then
		builtin printf "${Reply#DEVICE_REGISTERED }"
	else
		builtin printf "E"
	fi
}

## Device communication functions
ReadFromDevice()
{
	local Encoding
	case "$DeviceProtocol_Encoding" in
		none)
			Encoding=raw
		;;
		enc)
			Encoding=enc
		;;
		hex)
			Encoding=hex
		;;
	esac

	if [[ "$DeviceProtocol_Type" == line ]]; then
		/usr/pluto/bin/fdread "--$Encoding" --line --eol "$DeviceProtocol_Separator" --fd "$Framework_FD_Device"
	else # stream
		/usr/pluto/bin/fdread "--$Encoding" --fd "$Framework_FD_Device"
	fi
}

SendToDevice()
{
	local Data="$1"

	local Encoding
	case "$DeviceProtocol_Encoding" in
		none)
			Encoding=raw
		;;
		enc)
			Encoding=enc
		;;
		hex)
			Encoding=hex
		;;
	esac

	Log "SendToDevice: $1"
	/usr/pluto/bin/fdwrite "--$Encoding" --fd "$Framework_FD_Device" --data "$1"
	if [[ "$DeviceProtocol_AutoAppendSeparator" == yes ]]; then
		/usr/pluto/bin/fdwrite "--$Encoding" --fd "$Framework_FD_Device" --data "$DeviceProtocol_Separator"
	fi
	if [[ -n "$DeviceProtocol_Delay" ]]; then
		sleep "$DeviceProtocol_Delay"
	fi
}

FdWatch()
{
	local Fd="$1" Function="$2"

	WatchFD="$WatchFD $Fd"
	export -n "FdAction_$Fd=$Function"
}

FdStopWatching()
{
	local Fd="$1"
	local FdIdx
	local NewWatchFD

	for FdIdx in $WatchFD; do
		if [[ "$FdIdx" == "$Fd" ]]; then
			continue
		fi
		NewWatchFD="$NewWatchFD $FdIdx"
	done
	WatchFD="$NewWatchFD"
}

## Message handling functions
ParmEncode()
{
	local Parm="$1"
	php -r "echo rawurlencode('$Parm');"
}

ParmDecode()
{
	local Parm="$1"
	php -r "echo urldecode('$Parm');"
}

BuildCommand()
{
	local Parm

	local -a EncParm=()

	for Parm in "$@"; do
		Parm=$(ParmEncode "$Parm")
		EncParm=("${EncParm[@]}" "$Parm")
	done

	builtin echo "${EncParm[@]}"
}

## Utility functions
Log()
{
	local LogMsg="$(date --rfc-3339=ns) $*"
	builtin echo "$LogMsg" >&2
	if [[ -n "$Framework_LogFile" ]]; then
		builtin echo "$LogMsg" >&$Framework_LogFD
	fi
}

##############################################################################
## Internal functions
## Make sure no echo messes things up by mistake
echo()
{
	builtin echo "$@" >&2
}

## And neither does printf
printf()
{
	builtin printf "$@" >&2
}

Framework_FD_DCE=
Framework_FD_Device=

Framework_LoadParms()
{
	Router=dcerouter
	for ((i = 1; i <= "$#"; i++)); do
		case "${!i}" in
			-d) ((i++)); DevNo="${!i}" ;;
			-r) ((i++)); Router="${!i}" ;;
			-l) ((i++)); Framework_LogFile="${!i}"; exec {Framework_LogFD}>>"$Framework_LogFile" ;;
		esac
	done

	if [[ -z "$DevNo" ]]; then
		Log "ERROR: Device Number not specified"
		exit 1
	fi
}

Framework_MainLoop()
{
	local DataAvailable
	local FD
	local Quit=NoQuit
	local Command
	local Var

	Log "Main Loop"

	while [[ "$Quit" != Quit ]]; do
		DataAvailable=$(/usr/pluto/bin/fdselect -1 0 $WatchFD)
		Log "Data available: $DataAvailable"
		for FD in $DataAvailable; do
			Log "Processing fd $FD"
			Var="FdAction_$FD"
			Command="${!Var}"
			if [[ -n "$Command" ]]; then
				Log "fd $FD: command: $Command"
				$Command
			else
				Log "fd $FD: no command; doing nothing"
			fi
			## http://lists.gnu.org/archive/html/bug-bash/2010-04/msg00080.html
			(:) # <--- This acts like a garbage collector for leaked file descriptors
		done
	done
}

Framework_Init()
{
	trap Framework_OnExit EXIT

	Configure

	Framework_PipeDir="/usr/pluto/pipes/$DevNo"
	rm -rf "$Framework_PipeDir"
	mkdir -p "$Framework_PipeDir"

	Framework_DelayDCE="DontDelayDCE"
	Framework_DCEqueue=()
	Framework_PIDofDCEConn=
	Framework_PIDofDevConn=

	Framework_ConnectDCE
	Framework_ConnectDevice

	OnInit
}

Framework_OnExit()
{
	if [[ -n "$Framework_PIDofDevConn" ]]; then
		kill "$Framework_PIDofDevConn"
	fi
	OnExit
}

Framework_OpenFD()
{
	local VarFD="$1" File="$2"
	local FD

	while :; do
		sleep .5
		if exec {FD}<>"$File"; then
			export -n "$VarFD=$FD"
			return 0
		fi
	done

	export -n "$VarFD=-1"
	return 1
}

Framework_ConnectDCE()
{
	local PathToWhisperer="/usr/pluto/bin/DCE-Whisperer"
	Framework_DCE_ExitCodeFile="$Framework_PipeDir/DCE_ExitCode"

	socat PTY,link="$Framework_PipeDir/ttyDCE",echo=0,icanon=0,raw EXEC:"\"$PathToWhisperer\" -d \"$DevNo\" -r \"$Router\" -e \"$Framework_DCE_ExitCodeFile\"" &
	Framework_PIDofDCEConn=$!
	if Framework_OpenFD Framework_FD_DCE "$Framework_PipeDir/ttyDCE"; then
		FdWatch "$Framework_FD_DCE" Framework_ReadFromDCE
		return 0
	fi

	Log "Failed to start DCE Whisperer"
	exit 1
}

Framework_ConnectDevice()
{
	Log "ConnectDevice: Connecting device. Connection type: $DeviceConnection_Type"
	case "$DeviceConnection_Type" in
		none)
			Log "ConnectDevice: No device to connect to"
			return 0
		;;
		serial)
			if [[ -z "$DeviceConnection_BaudRate" || -z "$DeviceConnection_Parity" || -z "$DeviceConnection_SerialPort" ]]; then
				Log "ConnectDevice: Serial connection type with incomplete settings"
				exit 1
			fi
			if [[ ! -e "$DeviceConnection_SerialPort" || -d "$DeviceConnection_SerialPort" ]]; then
				Log "ConnectDevice: Invalid serial port"
			fi

			local FD_SerialPort
			exec {FD_SerialPort}<>"$DeviceConnection_SerialPort"
			Log "ConnectDevice: Parameters: Baud=$DeviceConnection_BaudRate Parity=$DeviceConnection_Parity Port=$DeviceConnection_SerialPort"
			/usr/pluto/bin/TestSerialPort -p "/dev/fd/$FD_SerialPort" -b "$DeviceConnection_BaudRate" -P "$DeviceConnection_Parity"
			socat PTY,link="$Framework_PipeDir/ttyDevice",echo=0,icanon=0,raw "/dev/fd/$FD_SerialPort" &
			Framework_PIDofDevConn=$!
			exec {FD_SerialPort}>&-

			# work around pl2303 driver bug (?)
			# - all sent data (but not any received data) is scrambled without this delay
			# - but only if we attempt to send data first; if we receive data first without the delay, the bug doesn't happen.
			# - once the bug happens, it never recovers during the session; reopenning the file and waiting does though
			sleep 0.1
		;;
		inet)
			Log "ConnectDevice: Parameters: Protocol=$DeviceConnection_Protocol Endpoint: $DeviceConnection_Endpoint"
			socat PTY,link="$Framework_PipeDir/ttyDevice",echo=0,icanon=0,raw "$DeviceConnection_Protocol:$DeviceConnection_Endpoint" &
			Framework_PIDofDevConn=$!
		;;
		custom)
			Log "ConnectDevice: Parameters: Command=$DeviceConnection_Command"
			socat PTY,link="$Framework_PipeDir/ttyDevice",echo=0,icanon=0,raw "EXEC:\"$DeviceConnection_Command\"" &
			Framework_PIDofDevConn=$!
		;;
		*)
			Log "ConnectDevice: Bad connection type '$DeviceConnection_Type'"
			exit 1
		;;
	esac

	if Framework_OpenFD Framework_FD_Device "$Framework_PipeDir/ttyDevice"; then
		FdWatch "$Framework_FD_Device" Framework_ReadFromDevice

		Log "ConnectDevice: Device connected successfully"
		OnDeviceConnect
	else
		Log "ConnectDevice: Device connection failed"
		exit 1
	fi
	return 0
}

## Fd Action
Framework_ReadFromDCE()
{
	local DCEline
	local -a DCECmd
	local Reply From To Type Cmd
	local Action
	local -a Parms
	local i line
	local ParmName ParmValue
	local CmdReplyFile ReplyString ReplyOutVars
	local -a CmdReply
	local ExitCode
	local Var

	CmdReplyFile="$Framework_PipeDir/CmdReply"

	Log "Framework_ReadFromDCE"
	DCEline=$(/usr/pluto/bin/fdread --raw --line --eol $'\n' --fd "$Framework_FD_DCE")
	if [[ -z "$DCEline" ]]; then
		Log "DCE whisperer exited. Shutting down."
		if [[ -f "$Framework_DCE_ExitCodeFile" ]]; then
			ExitCode=$(<"$Framework_DCE_ExitCodeFile")
			rm -f "$Framework_DCE_ExitCodeFile"
		fi
		if [[ -z "$ExitCode" ]]; then
			ExitCode=1
		fi
		exit "$ExitCode"
	else
		DCECmd=()
		## This is done this way in order to prevent path expansion on parameters containing stars and other shell pattern characters
		while read -d' ' line; do
			DCECmd=("${DCECmd[@]}" "$line")
		done <<<"$DCEline " # NOTE: space at the end required because we told 'read' that is the line separator
		Reply="${DCECmd[0]}"
		From="${DCECmd[1]}"
		To="${DCECmd[2]}"
		Type="${DCECmd[3]}"
		Cmd="${DCECmd[4]}"

		for ((i = 5; i < ${#DCECmd[@]}; i++)); do
			ParmName="${DCECmd[$i]}"
			((i++))
			
			ParmValue="${DCECmd[$i]}"
			if [[ "$ParmValue" == *%* ]]; then
				ParmValue=$(ParmDecode "$ParmValue") # this takes almost 100ms
			fi
			export -n "Parm_$ParmName=$ParmValue"
		done

		if [[ "$Type" == 1 ]]; then # this branch is for commands (i.e. not events or other stuff)
			## Queue all the messages that this function sends out
			## otherwise, the DCE router seems to get confused about the reply
			Framework_DelayDCE=DelayDCE
			if [[ "$DevNo" != "$To" ]]; then
				Log "Received DCE command for child $To with parms ${Parms[*]}"
				ReceivedCommandForChild "$From" "$To" "$Type" "$Cmd" >"$CmdReplyFile"
			elif declare -F "Cmd_$Cmd" &>/dev/null; then
				Log "Calling function for DCE command $Cmd with parms ${Parms[*]}"
				"Cmd_$Cmd" "$From" "$To" >"$CmdReplyFile"
			else
				Log "Received unknown DCE command $Cmd with parms ${Parms[*]}"
				ReceivedUnknownCommand "$From" "$To" "$Type" "$Cmd" >"$CmdReplyFile"
			fi
			for i in 0 1; do
				read line
				CmdReply[$i]="$line"
			done <"$CmdReplyFile"
			Framework_DelayDCE=DontDelayDCE

			ReplyString="${CmdReply[0]}"
			ReplyOutVars="${CmdReply[1]}"

			if [[ -z "$ReplyString" ]]; then
				ReplyString=OK
			fi
			if [[ "$Reply" == "-r" ]]; then
				SendToDCE "reply txt $ReplyString"
			elif [[ "$Reply" == "-o" ]]; then
				SendToDCE "reply msg $DevNo $From 4 0 0 $ReplyString $ReplyOutVars"
			fi

		# TODO: events and interceptors
		fi

		for Var in ${!Parm_*}; do
			unset $Var
		done

		Framework_SendDelayedDCE
	fi
}

Framework_ReadFromDevice()
{
	local Data

	Log "Processing incoming data from the device"

	Data=$(ReadFromDevice)

	if [[ -z "$Data" ]]; then
		Log "Connection to the device was lost"
		if [[ -n "$Framework_FD_Device" ]]; then
			FdStopWatching "$Framework_FD_Device"
			exec {Framework_FD_Device}>&-
			Framework_FD_Device=
		fi

		if declare -F "OnDeviceDisconnect" &>/dev/null; then
			OnDeviceDisconnect
		fi

		if [[ "$DeviceConnection_OnDisconnect" == reconnect ]]; then
			Framework_ConnectDevice
		else
			Framework_DisableDevice
			exit 0
		fi
	else
		ProcessDeviceStream "$Data"
	fi
}

Framework_SendDelayedDCE()
{
	local Cmd

	if [[ "$Framework_DelayDCE" == DelayDCE ]]; then
		Log "SendDelayedDCE: called while delay is still active"
		return
	fi
	Log "Sending out delayed DCE messages"

	for Cmd in "${Framework_DCEqueue[@]}"; do
		SendToDCE "$Cmd"
	done
	Framework_DCEqueue=()
}

Framework_DisableDevice()
{
	:
}

## Run
Framework_Run()
{
	Log "Running"
	Framework_LoadParms "$@"

	Log "Init"
	Framework_Init

	Framework_MainLoop
}

Framework_Run "$@"
