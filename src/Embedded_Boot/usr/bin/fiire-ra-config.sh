#!/bin/sh

USERNAME="$1"
PASSWORD="$2"
RA_DISABLED="$3"

CONF_FILE="/etc/fiire-ra/fiire-ra.conf"
SSH_KEY_FILE="/etc/fiire-ra/keys/fiire-ra.key"

PASSWORD_MD5="`echo -n "$PASSWORD" | md5sum | cut -d' ' -f1`"

log() {
	logger -s -t 'fiire-ra' $*
}

## Generate ssh keys
log 'Generating a new ssh key'
mkdir -p "`dirname "$SSH_KEY_FILE"`"
rm -f "${SSH_KEY_FILE}.tmp"
public_key=`dropbearkey -t rsa  -f "${SSH_KEY_FILE}.tmp" 2>/dev/null | grep ^ssh-rsa`
echo "$public_key" > "${SSH_KEY_FILE}.tmp.pub"

## Post the ssh-key/username/password-md5 to the website
log 'Sending information to fiire server'
HTTP_Response=`curl -k -F "username=$USERNAME" -F "password=$PASSWORD_MD5" -F "keyfile=@${SSH_KEY_FILE}.tmp.pub" "https://remote.fiire.com/uploadkey.php" 2>/dev/null`

## Verify the response 
case "$HTTP_Response" in
	'FAIL:'*)
		log "Error in response - `echo $HTTP_Response | cut -d':' -f2`"
		exit 1
	;;
	'OK:'*)
		PORT=`echo $HTTP_Response | cut -d':' -f2`
		log "got port $HTTP_Response"
	;;
	*)
		log "Error in response - fiire remote access server is not available"
		exit 2
	;;
esac

## Updating the configuration
log "Updating remove access configuration"
mv "$SSH_KEY_FILE.tmp" "$SSH_KEY_FILE"
mv "$SSH_KEY_FILE.tmp.pub" "$SSH_KEY_FILE.pub"

echo "#!/bin/bash"                  > "$CONF_FILE"
echo "RA_DISABLED=$RA_DISABLED" >> "$CONF_FILE"
echo "USERNAME=$USERNAME"  >> "$CONF_FILE"
echo "PASSWORD=$PASSWORD"  >> "$CONF_FILE"
echo "PORT=$PORT"          >> "$CONF_FILE"

chmod -R 700 "`dirname "$SSH_KEY_FILE"`"

/etc/init.d/fiire-ra restart
