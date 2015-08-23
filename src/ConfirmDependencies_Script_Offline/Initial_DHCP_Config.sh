#!/bin/bash

NetworkString=$1
NetworkStringDigits=$2

## IntIP + IntNetmask
IntIP="$NetworkString"

for i in `seq $NetworkStringDigits 2` ;do
	if [[ $i == "3" ]] ;then
		sDHCP="${IntIP}.1"
	else
		IntIP="${IntIP}.0"
	fi

done
sDHCP="${IntIP}.2-${IntIP}.128,${IntIP}.129-${IntIP}.199"

echo $sDHCP
