#!/bin/bash

while true; do
	if [[ -c "/dev/input/js0" ]] || [[ -c "/dev/input/js1" ]] || [[ -c "/dev/input/js2" ]] || [[ -c "/dev/input/js3" ]]; then
		sleep 5
	else
		exit 0
	fi
done
