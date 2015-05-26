VERSION_DATE=`/bin/date --utc +%0Y%0m%0d.%0k%0M`
VERSION_ID="1.0.7+cvs$VERSION_DATE"

echo Downloading vdr-xineliboutput $VERSION_ID from cvs...
cvs -z3 -d:pserver:anonymous@xineliboutput.cvs.sourceforge.net:/cvsroot/xineliboutput export -DNOW vdr-xineliboutput
tar cvfz "../vdr-plugin-xineliboutput_$VERSION_ID.orig.tar.gz" vdr-xineliboutput
rm -rf vdr-xineliboutput
dch -v "$VERSION_ID-1" "New Upstream Snapshot"
