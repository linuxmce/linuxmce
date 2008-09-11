#!/bin/bash

if [[ -n "$HEADER_SQL_Ops" ]]; then
	return 0
fi
HEADER_SQL_Ops=included

. /usr/pluto/bin/Config_Ops.sh

CSQL_DIR="/var/run/SQL_Ops"
CSQL_ID=""

#Example:
#  QUERY="SELECT field1, field2 FROM table"
#  RESULT=$(RunSQL "$QUERY")
#  for ROW in $RESULT; do
#      FIELD1=$(Field 1 "$ROW")
#      FIELD2=$(Field 2 "$ROW")
#      echo "Field1: $FIELD1; Field2: $FIELD2"
#  done

#Usage:
#  Var=$(RunSQL "<SQL query>")
#Returns resulting rows in $Var; you can iterate through them using 'for'
RunSQL()
{
	local Q Pass
	Q="$*"
	Pass=
	if [[ -n "$MySqlPassword" ]]; then
		Pass="-p$MySqlPassword"
	fi
	if [[ -n "$MySqlPort" ]]; then
		Port="-P $MySqlPort"
	fi
	if [[ -n "$Q" ]]; then
		echo "$Q;" | mysql -A -N "$SQL_DB" -h "$MySqlHost" -u "$MySqlUser" $Pass $Port | SQL_Translate
	fi
}

RunCSQL()
{
	local Q Q_id
	Q="$*"

	if [[ "$CSQL_ID" == "" ]] ;then
		RunSQL "$Q"
		return
	fi

	Q_id=$(echo "$Q" | sha1sum - | cut -d' ' -f1)
	if [[ ! -f "$CSQL_DIR/$CSQL_ID/$Q_id" ]] ;then
		RunSQL "$Q" > "$CSQL_DIR/$CSQL_ID/$Q_id"
	fi

	cat "$CSQL_DIR/$CSQL_ID/$Q_id"
}

PurgeCSQL() {
	CSQL_ID="$1"
	rm -rf "$CSQL_DIR/$CSQL_ID/"*
}

InitCSQL() {
	CSQL_ID="$1"
	mkdir -p "$CSQL_DIR/$CSQL_ID" 
}

#Usage:
#  Field=$(Field 1 "<SQL row>")
#Used in a 'for' loop to extract a field value from a row
Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d$'\x01' -f"$FieldNumber" | tr $'\x02' ' '
}

#Usage:
#  UseDB "<DB name>"
#Changes the default database
UseDB()
{
	SQL_DB="$1"
	if [[ -z "$SQL_DB" ]]; then
		SQL_DB="pluto_main"
	fi
}

UseDB "$MySqlDBName"

SQL_Translate()
{
	tr '\n\t ' $'\x20'$'\x01'$'\x02' | sed 's/ *$//'
}
