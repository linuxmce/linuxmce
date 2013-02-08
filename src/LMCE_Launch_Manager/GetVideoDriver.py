#!/usr/bin/python

import sys,xorgconfig

config = xorgconfig.readConfig("/etc/X11/xorg.conf")
devices = config.getSections("Device")

if len(sys.argv) > 1:
	outfile = open(sys.argv[1], "w")
	if outfile:
		outfile.write( devices[0].driver )
		outfile.close()

print devices[0].driver

