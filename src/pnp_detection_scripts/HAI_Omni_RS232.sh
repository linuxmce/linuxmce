#!/bin/bash

GetInstallation()
{
        local Q
        Q="Select FK_Installation FROM Device where PK_Device=1"
        Field 1 "$(RunSQL "$Q")"
}

GetCountry()
{
        local Q
        InstallID="$1"
        Q="SELECT Country.Description FROM Country
                INNER JOIN Installation on Country.PK_COuntry = Installation.FK_Country
                WHERE Installation.PK_Installation='$InstallID'"
        Field 1 "$(RunSQL "$Q")"
}

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

echo "HAI Omni detection script queue $2"
# /usr/pluto/bin/TestSerialPort -p /dev/ttyS0 -P N81 -b 9600 -t "\5A\01\05\C1\93" -s "Z\01\06\81\92" -i 4 # FOR TESTING
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\5A\01\05\C1\93" -s "Z\01\06\81\92" -i 4
if [[ "$?" -ne 0 ]]; then
        echo "It's not a HAI Omni"
        /usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
        echo "It is a HAI Omni"
        InstallID=$(GetInstallation)
        echo $InstallID
        Country=$(GetCountry "$InstallID")
        echo $Country

        if [ "$Country" = "UNITED STATES" -o "$Country" = "BELIZE" -o "$Country" = "JAMAICA" -o "$Country" = "UNITED STATES MINOR OUTLYING I" ]; then
                echo "Use Fahrenheit"
                /usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2116 109 "284|F"
                /usr/pluto/bin/MessageSend dcerouter -r -targetType category -bl 5 14 5 1 741 159 246 224 $2 163 "HAI Omni Series Panel" 125 1
        else
                echo "Use Celcius"
                /usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2116 109 "284|C"
                /usr/pluto/bin/MessageSend dcerouter -r -targetType category -bl 5 14 5 1 741 159 246 224 $2 163 "HAI Omni Series Panel" 125 1
        fi
fi

