#!/bin/bash

update-rc.d -f LMCEUpdate remove >/dev/null
update-rc.d -f LMCEUpdate start 98 2 3 4 5 . >/dev/null

