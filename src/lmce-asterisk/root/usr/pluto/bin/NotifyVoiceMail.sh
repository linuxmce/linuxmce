#!/bin/bash

context="$1"
extension="$2"
messages="$3"

if [[ -z "$extension" ]]; then
        echo "Empty extension"
        exit 1
fi

/usr/pluto/bin/SendVoiceMailEvent.sh ${extension}
