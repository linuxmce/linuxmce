#!/bin/sh

# pvrui.sh: PVRUSB2 control program.
# Uses the dialog program to display menus.
# Written by Phil Endecott, Feb 2006.
# This code is in the public domain; do whatever you like with it.

# Phil's contact information can be found at this URL:
# http://chezphil.org/email/genemail.cgi

# Please contact Phil directly for information about this utility

# Note (isely@pobox.com 2-Jul-2006) - This script is somewhat broken
# at the moment.  I will probably update it soon, but I just haven't
# have a chance yet.  If someone else would like to contribute an
# update that would be great...

sysclassroot=/sys/class/pvrusb2

progtitle="PVRUSB2 Settings"

dialogout=`mktemp`

trap "rm $dialogout" 0


ndevices=`ls $sysclassroot 2>/dev/null | wc -l`
case $ndevices in
   0) echo "No pvrusb2 devices found" ; exit 1 ;;
   1) device=`ls $sysclassroot` ;;
   *) devices=`ls $sysclassroot`
      devicelist=
      for dev in $devices
      do
        devicelist="$devicelist $dev $dev"
      done
      dialog \
        --backtitle "$progtitle" \
        --no-cancel \
        --menu "Select device:" \
        0 0 0 \
        $devicelist \
        2>$dialogout
      returncode=$?
      case $returncode in
      1) exit 1 ;;
      0) read device < $dialogout || true ;;
      esac ;;
esac

cd $sysclassroot/$device


while true
do

   controllist=""
   for setting in ctl_*
   do
     name=`cat $setting/name`
     cur_val=`cat $setting/cur_val`
     controllist="$controllist '$name' '$cur_val'"
   done
   eval "dialog \
     --backtitle '$progtitle' \
     --cancel-label 'Exit' \
     --menu 'Settings for $device' \
     0 0 0 \
     $controllist" \
     2>$dialogout
   returncode=$?
   case $returncode in
     1) exit 1 ;;
     0) read choice < $dialogout || true
   esac

   for setting in ctl_*
   do
     name=`cat $setting/name`
     if [ "$name" == "$choice" ]
     then
       break
     fi
   done

   ctype=`cat $setting/type`

   if [ "$ctype" == "enum" ]
   then

     enums=""
     for enum in `cat $setting/enum_val | tr ' ' _`
     do
       enums="$enums '$enum' ''"
     done
     current=`cat $setting/cur_val`
     eval "dialog \
       --backtitle '$progtitle' \
       --default-item '$current' \
       --menu '$device: $choice' \
       0 0 0 \
       $enums" \
       2>$dialogout
     returncode=$?
     case $returncode in
       1) ;;
       0) read value < $dialogout || true
          echo $value | tr _ ' ' > $setting/cur_val
     esac

   elif [ "$ctype" == "boolean" ]
   then

#     eval "dialog \
#       --backtitle '$progtitle' \
#       --yesno '$device: $choice' \
#       0 0 \
#       2>$dialogout
#     returncode=$?
#     case $returncode in
#       1) echo 0 > $setting/cur_val ;;
#       0) echo 1 > $setting/cur_val ;;
#     esac

     enums="'false' '' 'true' ''"
     current=`cat $setting/cur_val`
     eval "dialog \
       --backtitle '$progtitle' \
       --default-item '$current' \
       --menu '$device: $choice' \
       0 0 0 \
       $enums" \
       2>$dialogout
     returncode=$?
     case $returncode in
       1) ;;
       0) read value < $dialogout || true
          echo $value > $setting/cur_val
     esac

   else

     dialog \
       --backtitle "$progtitle" \
       --form "$device: $choice" \
       0 0 0 \
       "Min"     1 1 `cat $setting/min_val` 1 20 0 0 \
       "Max"     2 1 `cat $setting/max_val` 2 20 0 0 \
       "Current" 3 1 `cat $setting/cur_val` 3 20 12 0 \
       2>$dialogout
     returncode=$?
     case $returncode in
       1) ;;
       0) read value < $dialogout || true
          echo $value > $setting/cur_val
     esac

   fi

done
