#!/bin/bash

#TODO: activate a firstboot script here?

update-rc.d -f firstboot start 91 2 3 4 5 . >/dev/null
