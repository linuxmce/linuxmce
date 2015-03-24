#!/bin/sh

update-rc.d -f switch_cpu_governor start 50 2 3 4 5 . >/dev/null
