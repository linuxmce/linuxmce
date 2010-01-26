#! /bin/sh
#testcommand for media player
#parameters: command [filename] [...]
#  test
#  play filename xsize ysize
#return -1 if any error (no output!)
#this script is prepared for tools writing to stdout or requiring a file name
#  for tools writing to stdout simple create a new convertXXX function
#    that sends the tool to background and sets MPID to the pid
#  for tools requiring a filename call createFifo first, then
#    also create a function that sends the tool in background
#  after starting the conversion tool call waitFkt, this will kill the conversion
#  tool and the fifo reader if we are killed or if one of the tools finishes

#set -x

FIFO=/tmp/vdrfifo$$

log() {
  echo >/dev/null
  #echo "$$:`date `:$*" >> /tmp/test1.log
}

trapfkt() {
  log "trap received"
  if [ "$MPID" != "" ] ; then
    kill $MPID > /dev/null 2>&1
    kill -9 $MPID > /dev/null 2>&1
  fi
  if [ "$FPID" != "" ] ; then
    kill $FPID > /dev/null 2>&1
    kill -9 $FPID > /dev/null 2>&1
  fi
  if [ -p $FIFO ] ; then
    rm -f $FIFO
  fi
}

#create a fifo and start a cat
createFifo() {
  mkfifo $FIFO >/dev/null 2>&1 || exit 1
  cat $FIFO 2>&1 &
  FPID=$!
}
  
#convert AVI to mpeg2, suing mencoder see
#http://www.wiki.csoft.at/index.php/MEncoder_Scripts
convertAVI() {
   #mencoder  -oac lavc -ovc lavc -of mpeg -mpegopts format=dvd -vf harddup -srate 48000 -af lavcresample=48000 -lavcopts vcodec=mpeg2video:vrc_buf_size=1835:vrc_maxrate=9800:vbitrate=5000:keyint=15:acodec=mp2:abitrate=192 $RTOP -o $FIFO $1 > /dev/null 2>&1 &
   mencoder  -oac lavc -ovc lavc -of mpeg -mpegopts format=dvd -vf harddup -srate 48000 -af lavcresample=48000 -lavcopts vcodec=mpeg2video:vrc_buf_size=1835:vrc_maxrate=5000:vbitrate=3000:keyint=15:acodec=mp2:abitrate=192 $RTOP -o $FIFO $1 > /dev/null 2>&1 &
  MPID=$!
}

#convert WAV to mpeg using lame
convertWAV() {
lame -v -b 192 -S $1 - - 2>&1 &
MPID=$!
}

#convert pictures using GraphicsMagic
convertPicture(){
  if [ "$2" != "" -a "$3" != "" ] ; then
    fopt="-geometry ${2}x$3"
  fi
  gm convert $fopt "$1" jpeg:- 2>&1 &
  MPID=$!
}

#cat data
catData() {
  cat "$1" 2>&1 &
  MPID=$!
}

#convert a playlist
#handle http:// as audio-url
convertList() {
  cat "$1" 2>&1 | sed 's?^ *http:\(.*\)?/http:\1.http-audio?'
}

#get a HTTP url via wget
getHTTP() {
  wget -q -O - "$1" 2>&1 &
  MPID=$!
}





#wait until the cat or the converter have been stopped
waitFkt() {
  while [ 1 = 1 ]
  do
  dokill=0
  if [ "$MPID" != "" ] ; then
    kill -0 $MPID >/dev/null 2>&1 || dokill=1
  fi
  if [ "$FPID" != "" ] ; then
    kill -0 $FPID >/dev/null 2>&1 || dokill=1
  fi
  if [ $dokill = 1 ] ; then
    log "killing MPID=$MPID FPID=$FPID"
    kill -9 $MPID > /dev/null 2>&1
    MPID=""
    kill -9 $FPID > /dev/null 2>&1
    FPID=""
  fi
  if [ "$FPID" = "" -a "$MPID" = "" ] ; then
    log leaving waitFkt
    break
  fi
  usleep 100000
  done
  exit 0
}
    

log started with param "$*"
if [ "$1" = "" ] ; then
  echo inavlid call
  exit 1
fi
trap trapfkt 0 1 2 3 4 5 6 7 8 10 11 12 13 14 15
case $1 in
  check)
  exit 0
  ;;
  play)
  exts=`echo "$2" | sed 's/.*\.//'` 
  if [ "$exts" = "" ] ; then
    exit 1
  fi
  fname=`echo "$2" | sed 's/\.[^.]*$//'`
  case $exts in
    xmpg)
    #mpeg2 test
    nname="$fname.mpg"
    cat "$nname" &
    MPID=$!
    waitFkt
    ;;
    xjpg)
    #jpeg test
    nname="$fname.jpg"
    [ "$nname" = "" ] && exit 1
    [ ! -f "$nname" ] && exit 1
    convertPicture "$nname" $3 $4
    waitFkt
    ;;
    xmp3)
    #mp3 test
    nname="$fname.mp3"
    cat "$nname" &
    MPID=$!
    waitFkt
    ;;
    #real converting functions
    bmp|BMP|tiff|TIFF|png|PNG)
    convertPicture "$2" $3 $4
    waitFkt
    ;;
    AVI|avi)
    createFifo
    convertAVI "$2" $3 $4
    waitFkt
    ;;
    WAV|wav)
    convertWAV "$2"
    waitFkt
    ;;
    xdir)
    catData "$2"
    waitFkt
    ;;
    m3u|dir)
    convertList "$2"
    waitFkt
    ;;
    http-audio)
    fn=`echo "$2" | sed 's/\.[^.]*$//' | sed 's?^ */??'`
    getHTTP "$fn"
    waitFkt
    ;;
    *)
    exit 1
    ;;
  esac
  ;;
  *)
  exit 1
  ;;
esac
