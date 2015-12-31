#!/bin/bash

echo ""
echo "Pluto fix for North America DST"
echo ""

echo "Before:"
zdump -v /usr/share/zoneinfo/America/New_York | grep 2007

# create a temp directory to extract to.
export WRKDIR=`mktemp -d /tmp/selfextract.XXXXXX`

SKIP=$(awk '/^__ARCHIVE_FOLLOWS__/ { print NR + 1; exit 0; }' "$0")

# Take the TGZ portion of this file and pipe it to tar.
tail -n +"$SKIP" "$0" | tar xz -C "$WRKDIR"

# apply the fix
pushd "$WRKDIR" >/dev/null
zic -d zoneinfo northamerica
cd zoneinfo
cp -r * /usr/share/zoneinfo/

popd >/dev/null

# delete the temp files
rm -rf $WRKDIR

echo "After:"
zdump -v /usr/share/zoneinfo/America/New_York | grep 2007

exit 0

__ARCHIVE_FOLLOWS__
