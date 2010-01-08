#!/bin/bash  

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

function Usage 
{
	echo ""
	echo "Script:  $0"
	echo "Sumary:  This script will safely delete an entry from any table in the database"
	echo "         by first deleting all foreign key references, then deleting the entry"
	echo "         its self."
	echo "Usage:   $0 <database> <table> <primary_key> <optional dry_run_flag>"
	echo "Example: $0 pluto_main DeviceTemplate 53"
	echo "Note:    If the optional 4th parameter is 1, then a 'dry run' will"
	echo "         be performed. You will be shown what tables have foreign key references"
	echo "         but actual database deletions will NOT be performed!"
	echo ""
	exit 0
}
function Validate
{
	echo "Validating parameters..."
	UseDB "information_schema"

	# Check validity of the database
	Q="SELECT TABLE_NAME FROM TABLES WHERE TABLE_SCHEMA='$DATABASE'"
	R=$(RunSQL "$Q")
	if [[ $R == "" ]]; then
		echo "ERROR: Database $DATABASE does not exist!"
		echo ""
		exit 1
	fi

	# Check validity of the table
	Q="SELECT TABLE_NAME FROM TABLES WHERE TABLE_SCHEMA='$DATABASE' AND TABLE_NAME='$TABLE'"
	R=$(RunSQL "$Q")
	if [[ $R == "" ]]; then
		echo "ERROR: Table $TABLE does not exist in database $DATABASE!"
		echo ""
		exit 1
	fi
	echo "  Done!"
}

echo ""
if [ $# -ne "3" ]; then
	if [ $# -ne "4" ];then
		echo "Wrong Number of arguments!"
		Usage
	fi
fi

# Set up some "constants" we will use in the code
DATABASE=$1
TABLE=$2
PK=$3
Validate
PRIMARY_FIELD="PK_$TABLE"
FOREIGN_FIELD="FK_$TABLE"
DRY_RUN="$4"

if [[ $DRY_RUN -eq "1" ]]; then
	echo ""
	echo "##################################"
	echo "DRY RUN BEING PERFORMED!"
	echo "The database will NOT be modified!"
	echo "##################################"
	echo ""
fi

# Delete every entry with a FK reference from all tables in the database.
echo "Deleting $FOREIGN_FEILD references in $DATABASE..."
UseDB "information_schema"
Q="SELECT TABLE_NAME FROM TABLES WHERE TABLE_SCHEMA='$DATABASE' AND TABLE_NAME <> '$TABLE'"
R=$(RunSQL "$Q")

for thisTable in $R; do
	UseDB "information_schema"
	Q="SELECT COLUMN_NAME FROM COLUMNS WHERE TABLE_SCHEMA = '$DATABASE' AND TABLE_NAME='$thisTable' AND COLUMN_NAME='$FOREIGN_FIELD'"
	R2=$(RunSQL "$Q")

	for thisDeletion in $R2; do
		
		if [[ $DRY_RUN -eq "1" ]]; then
			echo "  $DATABASE.$thisTable has a foreign key reference!"
		else
			UseDB "$DATABASE"
			Q="DELETE FROM $thisTable WHERE $FOREIGN_FIELD='$PK'"
			echo "  Deleting from $DATABASE.$thisTable"
			RunSQL "$Q"
		fi
	done

done

# Now, lets delete the master record
echo "Deleting master record in $DATABASE.$TABLE.."
if [[ $DRY_RUN -eq "1" ]]; then
	echo "  $DATABASE.$TABLE has the primary key reference!"
else
	UseDB "$DATABASE"
	Q="DELETE FROM $TABLE WHERE $PRIMARY_FIELD='$PK'"
	RunSQL "$Q"
	echo "  Done!"
fi

echo ""

