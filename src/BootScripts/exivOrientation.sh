#!/bin/bash
# Usage: <cmd> file orientation
FILE="$1"
ORIENTATION="$2"
/usr/bin/exiv2 -M "set Exif.Image.Orientation $ORIENTATION" mo "$FILE"
