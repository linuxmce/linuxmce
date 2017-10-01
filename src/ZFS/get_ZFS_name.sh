#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

PK_Device=$1

Q="SELECT Description FROM Device WHERE PK_Device=$PK_Device"
echo $(RunSQL "$Q")
