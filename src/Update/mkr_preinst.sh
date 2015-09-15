#!/bin/bash

update-rc.d -f LCMEUpdate remove >/dev/null 2>/dev/null || :
[[ -f /etc/init.d/LMCEUpdate ]] && rm -f /etc/init.d/LCMEUpdate || :
