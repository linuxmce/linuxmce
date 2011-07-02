#!/bin/sh
for img in *.png; do
    filename=${img%.*}
    convert "$filename.png" "$filename.jpeg"
done
