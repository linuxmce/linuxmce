#!/bin/bash
# parameters:
# $1 todir - directory to create and put the links in
# $x files to link into above dir

DIR="$1"
if [[ "$DIR" == "" ]]; then
    exit 1
fi

MAGIC_FILE=".tmp_photo_link_dir"

if [ -e "$DIR" ]; then
    if [ -e "$DIR/$MAGIC_FILE" ]; then
	# if directory exists and it is ours (the magic file is there), delete all files
	rm "$DIR"/*
	touch "$DIR/$MAGIC_FILE"
    fi
else
    echo "create new dir"
    # Create tmp folder
    mkdir -p "$DIR" || exit 1;
    # mark folder to recognize it, and add a folder lock, or UpdateMedia will f**k up our files
    touch "$DIR/$MAGIC_FILE" || exit 1;
    touch "$DIR.folderlock" || exit 1;
fi

let i=0
for file in "$@";
do
    if [[ $i > 0 ]]; then
	NAME=`basename "$file"`
	if [ -e "$DIR/$NAME" ]; then
	    let R=0
	    while [ -e "$DIR/$R$NAME" ]; do
		let R=$R+1
	    done
	    NAME="$R$NAME"
	fi
	echo "ln -s $file $DIR/$NAME"
	ln -s "$file" "$DIR/$NAME"
    fi
    let i=$i+1
done