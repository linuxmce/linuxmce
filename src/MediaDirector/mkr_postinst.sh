#!/bin/bash

# activate firstboot script here

update-rc.d -f 90_start_interactor.sh start 90 2 3 4 5 . >/dev/null
update-rc.d -f firstboot start 91 2 3 4 5 . >/dev/null
