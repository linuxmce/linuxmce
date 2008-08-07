#!/bin/sh


BACKUP_STORAGE_URL="http://backup.fiire.com/put.php"

CRITICAL_CONF="/etc/config /etc/pluto/installation_number /etc/firewall.config /etc/httpd.conf /etc/passwd"
NONCRITICAL_CONF="/etc/pluto/ /etc/TZ /etc/TZ-full"

log() {
	logger -s -t 'backup-store' $*
}

BackupID=`date +%s`
InstallationID=`cat /etc/pluto/installation_number`

if [[ -f /etc/pluto/fresh_install ]]; then
    Fresh_Install=1
    log "Fresh Install 1"
else
    log "Fresh Install 0"
    Fresh_Install=0
fi

# Backup non-critical configs on fiire.com
log "Creating a backup of the non-critical config files."
cd /
tar zcf "/tmp/$BackupID.tgz" $NONCRITICAL_CONF

# Send non critical backup to Backup server
curl -k -F "InstallationID=$InstallationID" -F "BackupID=$BackupID" -F "Backup=@/tmp/$BackupID.tgz" "$BACKUP_STORAGE_URL"
rm -f "/tmp/$BackupID.tgz"
nvram set backupid="$BackupID"
nvram set fresh_install="$Fresh_Install"

# Backup critical configs in nvram
log "Creating a backup of the critical config files."
cd /
nvram set backup="`tar zc $CRITICAL_CONF | uuencode -`"

nvram commit
