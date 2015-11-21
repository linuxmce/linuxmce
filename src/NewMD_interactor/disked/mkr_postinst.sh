#!/bin/bash

# setup the interactor and the firstboot script
update-rc.d -f 90_start_interactor.sh defaults >/dev/null
