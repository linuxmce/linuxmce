#!/bin/bash
echo "sqlCVS-postcommit hook running"
MESSAGE="sqlCVS commit to repo $2 by $3 batch $1 - $4"
echo "Send to Golem: $MESSAGE"
wget "http://vt100.at/announce.php?text=$MESSAGE" -o /dev/null -O /dev/null
