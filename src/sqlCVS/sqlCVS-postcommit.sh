#!/bin/bash
# $1 = batchID, $2 = repo, $3 = user, $4 = comment
echo "sqlCVS-postcommit hook running"

# Make GolemIX shout !
URL="http://schema.linuxmce.org/sqlCVS/?site=batch/detail&repository=$2&batchid=$1"
URL=$(perl -MURI::Escape -e 'print uri_escape("'$URL'");')
MESSAGE="sqlCVS commit to repo $2 by $3 batch $1 - $4 ($URL)"
echo "Send to Golem: $MESSAGE"
wget "http://vt100.at/announce.php?text=$MESSAGE" -o /dev/null -O /dev/null

# Insert commit into trac ticket
