#!/bin/bash

# Don't restore sessions when firefox dies
if [[ -f /root/.mozilla/firefox/pluto.default/prefs.js ]] ;then
	grep -q "browser.sessionstore.enabled" /root/.mozilla/firefox/pluto.default/prefs.js ||
	echo 'user_pref("browser.sessionstore.enabled", false);' >> /root/.mozilla/firefox/pluto.default/prefs.js
fi

# Be sure to export bookmarks when firefox exits.
if [[ -f /root/.mozilla/firefox/pluto.default/prefs.js ]] ;then
	grep -q "browser.bookmarks.autoExportHTML" /root/.mozilla/firefox/pluto.default/prefs.js ||
	echo 'user_pref("browser.bookmarks.autoExportHTML", true)' >> /root/.mozilla/firefox/pluto.default/prefs.js
fi
 
. /usr/pluto/bin/Config_Ops.sh
export DISPLAY=":${Display}"

User="$1"
URL="$2"

LogFile="/var/log/pluto/mozilla.log"

DefaultProfileTxt="[General]
StartWithLastProfile=1

[Profile0]
Name=default
IsRelative=1
Path=pluto.default"

Section=
Loop="yes"
Reinstate=no
while [[ "$Loop" == yes ]]; do
	if [[ ! -f ~/.mozilla/firefox/profiles.ini || "$Reinstate" == yes ]]; then
		echo "$(date -R) MSG: Setting up profile config and profile directory" >>"$LogFile"
		if [[ ! -d ~/.mozilla/firefox/pluto.default ]]; then
			rm -rf ~/.mozilla/firefox/pluto.default
		fi
		rm -rf ~/.mozilla/firefox/profiles.ini

		mkdir -p ~/.mozilla/firefox/pluto.default
		echo "$DefaultProfileTxt" >~/.mozilla/firefox/profiles.ini
	fi

	echo "$(date -R) MSG: Reading profile config" >>"$LogFile"
	while read line; do
		if [[ "$line" == '['Profile*']' ]]; then
			if [[ -n "$Section" && "$Default" == 1 ]]; then
				break
			fi
			Section="$line"
			Name=
			IsRelative=
			Path=
			Default=
		elif [[ -n "$Section" && -n "$line" ]]; then
			Var="${line%%=*}"
			Value="${line#*=}"
			eval "$Var='$Value'"
		fi
	done <~/.mozilla/firefox/profiles.ini

	if [[ -n "$Path" ]]; then
		echo "$(date -R) OK: Profile config seems ok. Continuing" >>"$LogFile"
		Loop=no # profile ok. exit loop and continue
	elif [[ "$Reinstate" == no ]]; then
		echo "$(date -R) ERROR: Profile path is empty. Reinstating profile config" >>"$LogFile"
		Reinstate=yes # profile not ok, reinstate with default, re-read
	else
		echo "$(date -R) FATAL: Profile path is empty even after reinstating the config. Something wrong with the default?" >>"$LogFile"
		exit 1 # profile still not ok. if this happens, someone changed the above default
	fi
done

FireFoxProfile=~/".mozilla/firefox/$Path/"
cp /home/user_$User/bookmarks.html "$FireFoxProfile/bookmarks.html"

echo "$(date -R) user $User URL $URL" >>"$LogFile"

echo "starting firefox" >>"$LogFile"
firefox "$URL"
if [[ $User != 0 ]]; then
	cp "$FireFoxProfile/bookmarks.html" /home/user_$User
fi
echo "$(date -R) firefox ended" >>"$LogFile"
