#!/bin/bash

HALDevice="$1"
PNPQueue="$2"
SerialPort="$3"
ScriptName="$4"
InsteonDCETemplate=1932

IsNotInsteon()
{
        echo "It's not an Insteon PLM"
        /usr/pluto/bin/MessageSend dcerouter -r 0 $HALDevice 1 806 224 $PNPQueue 13 "$ScriptName" 44 0
        exit 0
}

IsInsteon()
{
        ModelNumber="$1"
        echo "It is an Insteon PLM Model $ModelNumber"
        /usr/pluto/bin/MessageSend dcerouter -r 0 $HALDevice 1 806 224 $PNPQueue 13 "$ScriptName" 44 $InsteonDCETemplate
        exit 0

}

echo "Insteon Powerline Modem Detection Script queue $PNPQueue on port $SerialPort"

ReturnString=$(/usr/pluto/bin/TestSerialPort -p $SerialPort -P N81 -b 19200 -t "\02\60"  -i 1)

if [[ -z "$ReturnString" ]]; then
        IsNotInsteon
elif [[ $ReturnString == *"\03\05"* ]]; then
        IsInsteon "2412S"
elif [[ $ReturnString == *"\03\11"* ]]; then
        IsInsteon "2413S"
elif [[ "$ReturnString" == *"\03\0B"* ]]; then
        IsInsteon "2412U"
elif [[ "$ReturnString" == *"\03\15"* ]]; then
        IsInsteon "2413U"
fi
