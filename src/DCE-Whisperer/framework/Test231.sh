#!/bin/bash

## Mandatory functions
Configure()
{
	## Device connection generics
	DeviceConnection_Type=none # choices: none, serial, inet, custom
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
	DeviceConnection_OnDisconnect=disable # choices: disable, reconnect

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
	echo "$Data"
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
	:
}

OnExit()
{
	:
}

## Command functions
# Parameters come as environment variables of this form: Parm_<Number>
Cmd_192()
{
	local From="$1" To="$2"
	echo "Cmd192 ($From -> $To): $Parm_50 : $Parm_29"
	SendToDCE "$DevNo 331 1 193 29 $Parm_29 50 $Parm_50"
	ReplyToDCE "OK" "21 Cmd192"
}

Cmd_193()
{
	local From="$1" To="$2"
	echo "Cmd193 ($From -> $To): $Parm_29 / $Parm_50"
	ReplyToDCE "OK" "20 Cmd193"
}

Cmd_100()
{
	local Reply

	SendToDevice "CHAN"
	Reply=$(ReadFromDevice)
	echo "Device said: $Reply"

	ReplyToDCE "OK" ""
}

. ./DCE-connect.sh "$@"
