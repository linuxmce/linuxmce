#!/bin/bash  

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

function Usage 
{
	echo ""
	echo "Script:  $0"
	echo "Sumary:  This script will safely delete an entry from any table in the database"
	echo "         by first deleting all foreign key references, external key references," 
	echo "         then deleting the entry its self."
	echo "Usage:   $0 <coma_separated_database_list> <table> <primary_key> <optional dry_run_flag>"
	echo "Example: $0 pluto_main,pluto_media DeviceTemplate 53"
	echo "Note:   The first parameter can be a single database, or a comma-separated list"
	echo "         of databases. The first database in the list is treated as the master" 
	echo "         database containing the table with the primary key. All other databases"
	echo "         in the list will be check for external keys"
	echo ""
	echo "         If the optional 4th parameter is 1, then a 'dry run' will"
	echo "         be performed. You will be shown what tables have foreign key references"
	echo "         but actual database deletions will NOT be performed!"
	echo ""
	echo ""
	exit 0
}
function Validate
{
	echo "Validating parameters..."
	UseDB "information_schema"

	for thisDatabase in $DATABASES; do
		# Check validity of the database
		Q="SELECT TABLE_NAME FROM TABLES WHERE TABLE_SCHEMA='$thisDatabase'"
		R=$(RunSQL "$Q")
		if [[ $R == "" ]]; then
			echo "ERROR: Database $thisDatabase does not exist!"
			echo ""
			exit 1
		fi
	done

	# Check validity of the table
	Q="SELECT TABLE_NAME FROM TABLES WHERE TABLE_SCHEMA='$MASTER_DB' AND TABLE_NAME='$TABLE'"
	R=$(RunSQL "$Q")
	if [[ $R == "" ]]; then
		echo "ERROR: Table $TABLE does not exist in database $MASTER_DB!"
		echo ""
		exit 1
	fi
	echo "  Done!"
	echo ""
}

echo ""
if [ $# -ne "3" ]; then
	if [ $# -ne "4" ];then
		echo "Wrong Number of arguments!"
		Usage
	fi
fi

# Set up some "constants" we will use in the code
DATABASES="$1"
DATABASES=$(echo $DATABASES | tr ',' ' ') #convert from comma-separated to space-separated
TABLE="$2"
PK="$3"
DRY_RUN="$4"
read MASTER_DB last <<< $DATABASES #make the first database the master database
Validate
PRIMARY_FIELD="PK_$TABLE"
FOREIGN_FIELD="FK_$TABLE"
EXTERNAL_FIELD="EK_$TABLE"


if [[ $DRY_RUN -eq "1" ]]; then
	echo ""
	echo "##################################"
	echo "DRY RUN BEING PERFORMED!"
	echo "The database will NOT be modified!"
	echo "##################################"
	echo ""
fi

for thisDatabase in $DATABASES; do
	if [[ $thisDatabase == $MASTER_DB ]]; then
		FIELD=$FOREIGN_FIELD
	else
		FIELD=$EXTERNAL_FIELD
	fi

	# Delete every entry with a FK reference from all tables in the database.
	echo "Deleting $FIELD references in $thisDatabase..."
	UseDB "information_schema"
	Q="SELECT TABLE_NAME FROM TABLES WHERE TABLE_SCHEMA='$thisDatabase' AND TABLE_NAME <> '$TABLE'"
	R=$(RunSQL "$Q")

	for thisTable in $R; do
		UseDB "information_schema"
		Q="SELECT COLUMN_NAME FROM COLUMNS WHERE TABLE_SCHEMA = '$thisDatabase' AND TABLE_NAME='$thisTable' AND COLUMN_NAME='$FIELD'"
		R2=$(RunSQL "$Q")

		for thisDeletion in $R2; do
		
			if [[ $DRY_RUN -eq "1" ]]; then
				echo "  $thisDatabase.$thisTable has a $FIELD field!"
			else
				UseDB "$thisDatabase"
				Q="DELETE FROM $thisTable WHERE $FIELD='$PK'"
				echo "  Deleting from $thisDatabase.$thisTable"
				RunSQL "$Q"
			fi
		done
	done
	echo "  Done!"
	echo ""
done

# Now, lets delete the master record
echo "Deleting master record in $MASTER_DB.$TABLE..."
if [[ $DRY_RUN -eq "1" ]]; then
	echo "  $MASTER_DB.$TABLE has the primary key reference!"
else
	UseDB "$MASTER_DB"
	Q="DELETE FROM $TABLE WHERE $PRIMARY_FIELD='$PK'"
	RunSQL "$Q"
fi
echo "  Done!"
echo ""

