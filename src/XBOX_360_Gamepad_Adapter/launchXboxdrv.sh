#!/bin/bash
rmmod xpad
xboxdrv --mimic-xpad-wireless -w $1
