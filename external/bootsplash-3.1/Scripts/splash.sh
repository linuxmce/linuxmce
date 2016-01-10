#!/bin/bash
#
# splash.sh - This shell script triggers bootsplash actions during
# system boot/shutdown. It can be run by the init scripts repeatedly
# specifying the currently executed system script.
# 
# This script is used to play animations/sounds/show text/move the
# progress bar, etc.
# 
# We call this function from /etc/rc.status:rc_splash() 
#
# This program is free software; you may redistribute it under 
# the terms of the GNU General Public License. This program has 
# absolutely no warranty.
#
# written 2002-2003 Stefan Reinauer, <stepan@suse.de>
# modified by Razvan Gavril, <razvan.g@plutohome.com>
#
# this script expects the following environment variables for an operable
# progress bar:
#
#  sscripts = number of start scripts to be executed for runlevel change
#  kscripts = number of stop scripts to be executed for runlevel change
#  progress = number of currently executed start/stop script
#  RUNLEVEL = runlevel to be reached.
#
# To play animations, it's advised that you have an trigers.cfg in your
# theme's config directory specifying the initscript name for an event and
# the command to be executed:
#
#   fsck start:bootanim start bounce.mng
#   ending:bootanim stop
#
# See http://www.bootsplash.org/ for more information.
#

_procsplash="`cat /proc/splash 2>/dev/null`"

# source our config
source /etc/default/bootsplash 2>/dev/null

# default settings
test -z "${THEME}" && THEME="default"
test ! -d "/etc/bootsplash/themes/$THEME" && exit 0

# parse trigers.cfg. This part should not be handled by the
# splash binary.

if [ -f /etc/bootsplash/themes/$THEME/config/trigers.cfg ]; then
  COMMAND="`cat /etc/bootsplash/themes/$THEME/config/trigers.cfg| grep "^$1"|cut -f2 -d\:`"
  eval $COMMAND
fi

# assertions
test -r /proc/splash || exit 0
test -z "$progress" -a -z "$num" && exit 0
test -z "`echo $_procsplash|grep on`" && exit 0

# We chose verbose in grub
test "`cat /proc/cmdline |grep \"splash=silent\"`" == "" && exit 0 

# acquire data
# 
num=$(( $sscripts + 2 ))
_shutdown="no"
_silent="no"
test "`cat /proc/splash |grep silent`" && _silent="yes"
test "$RUNLEVEL" == "6" -o "$RUNLEVEL" == "0" && _shutdown="yes"
test "$_shutdown" == "yes" && num=$(( $kscripts + 2 ))

function box() { true; } # ignore box descriptions in the config file

test -f "/etc/bootsplash/themes/$THEME/config/bootsplash-`fbresolution`.cfg" && \
  . /etc/bootsplash/themes/$THEME/config/bootsplash-`fbresolution`.cfg

#
# Print text string. (Booting/Shutting down the system. Press
# F2 for verbose mode)
#

if [ "$progress" == 1 -o "$1" == "splash start" ]; then
  test "$_shutdown" == "yes" && ( echo "silent" >/proc/splash; chvt 1)
  _boot="Booting"; test "$_shutdown" == "yes" && _boot="Shutting down"

  if [ "$text_x" != "" -a "$text_y" != "" \
       -a "$text_color" != "" -a "$text_size" != "" -a "$_silent" == "yes" ]; 
  then
    fbtruetype -x $text_x -y $text_y -t $text_color -s $text_size \
  			"$_boot the system... Press F2 for verbose mode"
  fi
fi

#
# Now paint the progressbar. This is done via the proc 
# interface of the bootsplash.
#

test -z "$progress_enable" && exit 0
echo "show $(( 65534 * ( $progress + 1 ) / $num ))" > /proc/splash

