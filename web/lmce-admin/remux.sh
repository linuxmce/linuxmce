#!/bin/bash
FULLPATH=""


  while getopts f:l opt; do
  case $opt in
  f) 
    FULLPATH="$OPTARG"
    ;;
   l)
    echo "rez not done"
    ;;
  esac
done

FNAME=$(basename "$FULLPATH")
DIRNAME=${FNAME%.*}

if [ -d "/tmp/$DIRNAME" ]; then
echo "file exists, not remuxing"
else
echo $FULLPATH
mkdir -p "/tmp/$DIRNAME"
#ffmpeg -y -i "$FULLPATH" -vcodec copy -acodec aac -b:a 192k -strict -2 "/tmp/$DIRNAME.mp4"  # </dev/null >/dev/null 2>/dev/null
 ffmpeg -v 9 -loglevel 99 -re -i "$FULLPATH" -an -c:v libx264 -b:v 128k -acodec aac -b:a 192k -strict -2 -flags -global_header -map 0  -f segment -segment_list "/tmp/$DIRNAME/$DIRNAME.m3u8"  "/tmp/$DIRNAME/stream%05d.mp4"

fi
echo "/tmp/$DIRNAME/$DIRNAME.m3u8"

exit 
