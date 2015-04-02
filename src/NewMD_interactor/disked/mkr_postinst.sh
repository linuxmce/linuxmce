#!/bin/bash

# setup the interactor and the firstboot script
update-rc.d -f 90_start_interactor.sh start 90 2 3 4 5 . >/dev/null
