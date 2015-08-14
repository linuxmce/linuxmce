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


filename=$(basename "$FULLPATH")
extension="${filename##*.}"
filename="${filename%.*}"



if [ -e "/tmp/$DIRNAME.mp4" ]; then
echo "file exists, not remuxing"
else
echo $FULLPATH

ffmpeg -i "$FULLPATH" -vcodec copy -acodec copy "/tmp/$filename.mp4" >/dev/null >/dev/null 2>/dev/null

#kinda ready
#ffmpeg -i "$FULLPATH" -c:v libx264 -b:v 128k -acodec aac -b:a 192k -strict -2 "/tmp/$filename.mp4"  # </dev/null >/dev/null 2>/dev/null

 #totally not ready
 #mkdir -p "/tmp/$DIRNAME"
 #ffmpeg -v 9 -loglevel 99 -re -i "$FULLPATH" -an -c:v libx264 -b:v 128k -acodec aac -b:a 192k -strict -2 -flags -global_header -map 0  -f segment -segment_list "/tmp/$DIRNAME/$DIRNAME.m3u8"  "/tmp/$DIRNAME/stream%05d.mp4"

fi
echo "/tmp/$filename.mp4"

exit 
