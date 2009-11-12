#! /bin/sh

filelist="mediaprovider.h media.h media.cc mediaplayer.h mediaplayer.cc mediafile.h mediafile.cc serialize.h serialize.cc vdrcommand.h mediaproviderids.h"
usage() {
  echo "usage: $0 [-f] [clientdir]"
  echo "   if clientdir is not given VOMPCLIENT has to be set"
}

error() {
  echo "--ERROR-- "$*
  exit 1
}

#p1 - in name
translateName() {
  echo $1 | sed 's/\.cc$/.c/'
}

if [ "$1" = "-h" ] ; then
  usage
  exit 0
fi

if [ "$1" = "-f" ] ; then
  force=1
  shift
fi

clientdir=$1
if [ "$clientdir" = "" ] ; then
  clientdir=$VOMPCLIENT
fi
if [ "$clientdir" = "" ] ; then
  echo neither clientdir provided nor VOMPCLIENT set
  usage
  exit 1
fi
backupdir=../vompserver-backup
if [ "$VOMPSERVERBACKUP" != "" ] ; then
  backupdir=$VOMPSERVERBACKUP
fi

be=`date +%Y%m%d%H%M%S`

backupdir=$backupdir/$be
echo "backing up to $backupdir"
mkdir -p $backupdir || error "unable to create backup directorry $backupdir"

for file in $filelist
do
  [ ! -f $clientdir/$file ] && error "$clientdir/$file not found"
done

for file in $filelist
do
  ofile=`translateName $file`
  if [ -f $ofile -a ! -L $ofile ] ; then
    echo backing up $ofile to $backupdir/$ofile
    cp $ofile $backupdir/$ofile || error "failed to backup $ofile"
    rm -f $ofile
  fi
  if [ "$force" = 1 ] ; then
    rm -f $ofile
  fi
  if [ ! -L $ofile ] ; then
    echo linking $clientdir/$file $ofile
    ln -s $clientdir/$file $ofile || error "unable to link $clientdir/$file to $ofile"
  else
    echo $ofile already exists as link, skip
  fi
done


