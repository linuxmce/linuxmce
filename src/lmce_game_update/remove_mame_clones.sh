#!/bin/sh
echo -n "Removing MAME clones from current directory..."
for i in $(mame -listclones | cut -d " " -f1); do rm -v $i.zip; done
echo "Done."
