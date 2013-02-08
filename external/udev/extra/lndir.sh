#! /bin/sh

set -e

# lndir - create shadow link tree
#
# $XConsortium: lndir.sh,v 1.10 92/03/16 17:52:17 gildea Exp $
#
# Used to create a copy of the a directory tree that has links for all
# non-directories (except those named RCS, SCCS or CVS.adm).  If you are
# building the distribution on more than one machine, you should use
# this technique.
#
# If your master sources are located in /usr/local/src/X and you would like
# your link tree to be in /usr/local/src/new-X, do the following:
#
# 	%  mkdir /usr/local/src/new-X
#	%  cd /usr/local/src/new-X
# 	%  lndir ../X

USAGE="Usage: $0 fromdir [todir]"

if [ $# -lt 1 -o $# -gt 2 ]
then
	echo "$USAGE"
	exit 1
fi

# First, Canonicalise the pathnames

DIRFROM="$(realpath $1)"

if [ $# -ne 2 ];
then
	DIRTO="."
else
	DIRTO="$2"
	if [ -e $DIRTO ]; then
		echo "Target directory exists."
		exit 2
	fi
fi

echo "$DIRTO:"
mkdir -p "$DIRTO"
DIRTO="$(realpath $DIRTO)"

if [ ! -d "$DIRFROM" ]
then
	echo "$0: $DIRFROM is not a directory"
	echo "$USAGE"
	exit 2
fi

if [ "$DIRFROM" = "$DIRTO" ]
then
	echo "$DIRFROM: FROM and TO are identical!"
	exit 1
fi

# parse output of "ls" below more carefully
IFS='
'
for file in `ls -af $DIRFROM`
do
    if [ "$DIRFROM/$file" ==  "$DIRTO" ]
    then
        continue
    fi
    if [ -d "$DIRFROM/$file" ] && [ ! -L "$DIRFROM/$file" ]
    then
	    case "$file" in
	        .|..|RCS|SCCS|CVS.adm)
		    continue
		    ;;
	    esac
	    sh $0 "$DIRFROM/$file" "$DIRTO/$file" 
    else
	    ln -s "$DIRFROM/$file" "$DIRTO"
    fi
done
