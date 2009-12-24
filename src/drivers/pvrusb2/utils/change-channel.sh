#!/bin/sh

# $Id: change-channel.sh 1690 2007-11-04 21:50:45Z isely $

# This sysfs based channel changing script was contributed on
# 10-Sep-2005 by Per Christian Henden <perchrh@pvv.org>.  Please see
# him or post to the pvrusb2 list if you have any questions about this
# script.  The text of his e-mail describing this script is pasted
# below:

#
# Hi,
#
# I made a script for changing channels (attached).
# Consider this a preview release :)
#
# It assumes a file mapping channel names to frequencies exist.
# The format is like this:
#
# [E5]
# freq = 175250
#
# The file can be made combining output from scantv with frequencies
# from /usr/local/share/xawtv/*.list
#
# I plan to automate the creation of this frequency file.
#
# It can be created like this:
# #goes to xawtv frequency lists
# cd /usr/local/share/xawtv
# #shows me the files that are included
# cat europe-west.list
# #concatenates the files that are included
# cat ccir-i-iii.list ccir-sl-sh.list  ccir-h.list uhf.list > ~/allids
# #extracts channel ids from scantv output
# cat ~/.tv/stations |grep "\["|cut -d "(" -f 2|cut -d")" -f 1 > myids
# #combines channel ids with frequencies
# (for id in `cat myids `; do grep -1 "\[$id\]" allids ;done) > freqs
#
# The placement of the file is given by the FREQUENCY_FILE variable in the
# script, so make sure you change that to point to your frequency file.
#
# -PER
#


#set -x #debug

FREQUENCY_FILE="/home/perchrh/.tv/freqs"
CHANNELID=$1

#check first if we are root
if [ $UID != "0" ]
   then
   echo "You need to be root to change channels"
   exit
fi

#check parameters
if [ $# == "0" ]
then
   echo "Usage is $0 \<channel id\>"
   echo "e.g. $0 S10"
   echo "Use $0 -list to get a list of available channels"
   exit
else if [ $1 == "-list" ]
then
  echo "Available channels are:"
  grep "\[" $FREQUENCY_FILE|tr -d "[,]"
  exit
fi
fi


freq=`grep  -1 -i "\[$CHANNELID\]" $FREQUENCY_FILE |tail -n 1|cut -d" " -f 3|tr -d " "`000

#check if we got an empty string returned from grep,
#that means the channel id was not found
if [ $freq == "000" ]
then
echo "Could not find channel with id $CHANNELID"
exit
else
echo "Found frequency $freq for channel $CHANNELID"
fi

#Change the channel for all pvrusb2 cards to that of $freq
if [ -x /sys/class/pvrusb2/ ]
then
ids=`find /sys/class/pvrusb2/ -maxdepth 1 -mindepth 1 -type d`
   for id in $ids
   do
   echo $freq > $id/ctl_frequency/cur_val
   done
else
echo "PVR USB2 sysfs interface not found"
fi

