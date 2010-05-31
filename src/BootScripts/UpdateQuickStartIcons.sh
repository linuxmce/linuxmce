#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

PIXDIR="/home/quick_start_icons"

if [[ ! -d "$PIXDIR" ]]; then
	mkdir -p "$PIXDIR"
fi

Q="SELECT PK_QuickStartTemplate,Icon FROM QuickStartTemplate Where Icon != ''"
R=$(RunSQL "$Q")

for Row in $R; do
	Template=$(Field 1 "$Row")
	Icon=$(Field 2 "$Row")
	LocalIcon="$PIXDIR/template_$Template.png"
	if [[ "$Icon" = "http://"* ]]; then
		# URL
		wget --tries=1 --timeout=2 -O "$PIXDIR/$Template" "$Icon"
		convert "$PIXDIR/$Template" "$LocalIcon"
		rm -f "$PIXDIR/$Template"
	elif [[ -n "$Icon" && -f "$Icon" ]]; then
		# File
		convert "$Icon" "$LocalIcon"
	fi
done

