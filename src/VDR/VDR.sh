#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

## Mandatory functions
Configure()
{
	## Device connection generics
	DeviceConnection_Type=inet # choices: none, serial, inet, custom
	## Connection parameters
	# type=serial:
	DeviceConnection_BaudRate=9600
	DeviceConnection_Parity=N81 # choices: N81, E81, O81
	DeviceConnection_SerialPort=/dev/ttyS1
	# type=inet:
	DeviceConnection_Protocol=TCP4 # choices: TCP4, TCP6, UDP4, UDP6
	DeviceConnection_Endpoint=127.0.0.1:2001 # Address:Port
	# type=custom:
	DeviceConnection_Command=

	## What to do when the device is lost
	DeviceConnection_OnDisconnect=reconnect # choices: disable, reconnect

	## Device protocol generics
	DeviceProtocol_Type=line # choices: line, stream
	# type=line:
	DeviceProtocol_Separator=$'\n' # must match encoding

	DeviceProtocol_Encoding=none # choices: none, enc, hex
	DeviceProtocol_AutoAppendSeparator=yes # choices: yes, no
}

ProcessDeviceStream()
{
	local Data="$1"
	echo "VDR said: $Data"
}

## Unknown command handlers
ReceivedCommandForChild()
{
	local From="$1" To="$2" Type="$3" Cmd="$4"
	ReplyToDCE "UNHANDLED" ""
}

ReceivedUnknownCommand()
{
	local From="$1" To="$2" Type="$3" Cmd="$4"
	ReplyToDCE "UNHANDLED" ""
}

## Hooks
OnDeviceConnect()
{
	local Hello
	Hello=$(ReadFromDevice) # hello message
	echo "Device said hello: $Hello"
}

OnInit()
{
	## User configuration code
	##########################

	VDRstatus=stopped
	vdr_sxfe_pid=

	UseAlternativeLibs
	if [[ -z "$DISPLAY" ]]; then
		export DISPLAY=":$Display"
	fi
	SendToDCE "$DevNo -1001 2 12 9 0 37 0" # 4 '' 9 0 37 0 # Playback Completed
	killall -9 vdr-sxfe
	sleep .5
}

OnExit()
{
	if [[ -n "$vdr_sxfe_pid" ]]; then
		kill -9 "$vdr_sxfe_pid"
	fi
}

## Command functions
# Parameters come as environment variables of this form: Parm_<Number>

## Play Media
Cmd_37()
{
	local From="$1" To="$2"

	local StreamID="$Parm_41"
	echo "Play Media: StreamID=$StreamID"
	if [[ "$VDRstatus" == stopped ]]; then
		VDRstatus=playing
		/usr/bin/vdr-sxfe --reconnect --fullscreen xvdr://127.0.0.1:37890 &>/dev/null </dev/null &
		#/usr/bin/vdr-sxfe --reconnect --fullscreen xvdr://127.0.0.1:37890 1>&2 </dev/null &
		vdr_sxfe_pid=$!
	fi

	ReplyToDCE "OK" ""
}

## Stop Media
Cmd_38()
{
	local From="$1" To="$2"

	if [[ -n "$vdr_sxfe_pid" ]]; then
		kill -9 "$vdr_sxfe_pid"
	fi
	vdr_sxfe_pid=
	VDRstatus=stopped

	ReplyToDCE "OK" ""
}

Cmd_204()
{
	local From="$1" To="$2"
	SendToDevice "HITK 0"
	ReplyToDCE "OK"
}

Cmd_205()
{
	local From="$1" To="$2"
	SendToDevice "HITK 1"
	ReplyToDCE "OK"
}

Cmd_206()
{
	local From="$1" To="$2"
	SendToDevice "HITK 2"
	ReplyToDCE "OK"
}

Cmd_207()
{
	local From="$1" To="$2"
	SendToDevice "HITK 3"
	ReplyToDCE "OK"
}

Cmd_208()
{
	local From="$1" To="$2"
	SendToDevice "HITK 4"
	ReplyToDCE "OK"
}

Cmd_209()
{
	local From="$1" To="$2"
	SendToDevice "HITK 5"
	ReplyToDCE "OK"
}

Cmd_210()
{
	local From="$1" To="$2"
	SendToDevice "HITK 6"
	ReplyToDCE "OK"
}

Cmd_211()
{
	local From="$1" To="$2"
	SendToDevice "HITK 7"
	ReplyToDCE "OK"
}

Cmd_212()
{
	local From="$1" To="$2"
	SendToDevice "HITK 8"
	ReplyToDCE "OK"
}

Cmd_213()
{
	local From="$1" To="$2"
	SendToDevice "HITK 9"
	ReplyToDCE "OK"
}

Cmd_190()
{
	local From="$1" To="$2"
	SendToDevice "HITK Ok"
	ReplyToDCE "OK"
}

Cmd_964()
{
	local From="$1" To="$2"
	SendToDevice "HITK Channel+"
	ReplyToDCE "OK"
}

Cmd_965()
{
	local From="$1" To="$2"
	SendToDevice "HITK Channel-"
	ReplyToDCE "OK"
}

Cmd_126()
{
	local From="$1" To="$2"
	SendToDevice "HITK Schedule"
	ReplyToDCE "OK"
}

Cmd_63()
{
	local From="$1" To="$2"
	SendToDevice "HITK Next"
	ReplyToDCE "OK"
}

Cmd_64()
{
	local From="$1" To="$2"
	SendToDevice "HITK Prev"
	ReplyToDCE "OK"
}

Cmd_92()
{
	local From="$1" To="$2"
	SendToDevice "HITK Pause"
	ReplyToDCE "OK"
}

Cmd_93()
{
	local From="$1" To="$2"
	SendToDevice "HITK FastFwd"
	ReplyToDCE "OK"
}

Cmd_125()
{
	local From="$1" To="$2"
	SendToDevice "HITK FastRew"
	ReplyToDCE "OK"
}

Cmd_95()
{
	local From="$1" To="$2"
	SendToDevice "HITK Stop"
	ReplyToDCE "OK"
}

Cmd_139()
{
	local From="$1" To="$2"
	SendToDevice "HITK Play"
	ReplyToDCE "OK"
}

Cmd_240()
{
	local From="$1" To="$2"
	SendToDevice "HITK Back"
	ReplyToDCE "OK"
}

Cmd_548()
{
	local From="$1" To="$2"
	SendToDevice "HITK Recordings"
	ReplyToDCE "OK"
}

Cmd_102()
{
	local From="$1" To="$2"
	SendToDevice "HITK Record"
	ReplyToDCE "OK"
}

Cmd_123()
{
	local From="$1" To="$2"
	SendToDevice "HITK Info"
	ReplyToDCE "OK"
}

Cmd_200()
{
	local From="$1" To="$2"
	SendToDevice "HITK Up"
	ReplyToDCE "OK"
}

Cmd_201()
{
	local From="$1" To="$2"
	SendToDevice "HITK Down"
	ReplyToDCE "OK"
}

Cmd_202()
{
	local From="$1" To="$2"
	SendToDevice "HITK Left"
	ReplyToDCE "OK"
}

Cmd_203()
{
	local From="$1" To="$2"
	SendToDevice "HITK Right"
	ReplyToDCE "OK"
}

Cmd_605()
{
	local From="$1" To="$2"
	SendToDevice "HITK Red"
	ReplyToDCE "OK"
}

Cmd_518()
{
	local From="$1" To="$2"
	SendToDevice "HITK Green"
	ReplyToDCE "OK"
}

Cmd_682()
{
	local From="$1" To="$2"
	SendToDevice "HITK Yellow"
	ReplyToDCE "OK"
}

Cmd_455()
{
	local From="$1" To="$2"
	SendToDevice "HITK Blue"
	ReplyToDCE "OK"
}

## Simulate Keypress
Cmd_28()
{
	local From="$1" To="$2"
	local KeyName PK_Button

	KeyName="$Parm_50"
	if [[ "$KeyName" == Power || "$KeyName" == Setup ]]; then
		KeyName=
	fi

	PK_Button="$Parm_26"
	case "$PK_Button" in
		15) KeyName=Record ;;
		16|17|18|19|20|21|22|23|24) KeyName="$((PK_Button-15))" ;;
		25) KeyName=0 ;;
		177) KeyName=Green ;;
		178) KeyName=Yellow ;;
		179) KeyName=Red ;;
		180) KeyName=Blue ;;
		215) KeyName=Pause ;;
		216) KeyName=Stop ;;
		217) KeyName=FastRew ;;
		218) KeyName=FastFwd ;;
		222) KeyName=Play ;;
		223) KeyName=Blue ;;
		224) KeyName=Yellow ;;
		225) KeyName=Green ;;
		226) KeyName=Red ;;
		227) KeyName=Info ;;
		6) KeyName=Channel+ ;;
		7) KeyName=Channel- ;;
	esac

	if [[ -n "$KeyName" ]]; then
		SendToDevice "HITK $KeyName"
	fi
	ReplyToDCE "OK"
}

## Start eveything up
. ./DCE-connect.sh "$@"
