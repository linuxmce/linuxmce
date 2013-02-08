#!/bin/bash

# Remove splash from current console (works on tty only)

TTY=$(readlink /proc/self/fd/0)
if [[ "$TTY" != "/dev/tty"? ]]; then
	echo "Not a tty. Aborting"
	exit 1
fi

Console=${TTY#/dev/tty}
((Console--))
splash -s -u $Console
kill -WINCH $PPID
