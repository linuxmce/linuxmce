#!/bin/bash


#  '
#  '## BEGIN : SectionName
#  '   Some line of data
#  '   And some other line of data
#  '## END : SectionName
#  '

TEMPLATE_FILES_DIRECTORY="/usr/pluto/templates"

## Creates and/or populates a section from a file
PopulateSection()
{
	local File="$1"
	local SectionName="$2"
	local SectionData="$3"

	## If the file exists and is not writable we should quit
	if [[ -f "$File" && ! -w "$File" ]] ;then
		echo "Section_Ops.sh: File $File is not writable !" >&2
		return
	fi

	## Create the file if it doesn't exist
	if [[ ! -f "$File" ]]; then
		touch "$File"
	fi

	## Check if we have a template file for this config and
	## try to see if is allready used
	TemplateFile="$TEMPLATE_FILES_DIRECTORY/$(basename $File).template"
	if [[ -f "$TemplateFile" ]] ;then
		isPlutoSectionedFileUsed="true"
		grep -q "## @FileType: Pluto Sectioned Config File ##" $File || isPlutoSectionedFileUsed="false"
		if [[ "$isPlutoSectionedFileUsed" != "true" ]] ;then
			cp -f $TemplateFile $File
		fi
	fi	
	
	## Check if our section exists in the file
	if ! grep -q "^## BEGIN : $SectionName" "$File"; then
		echo -e "\n## BEGIN : $SectionName\n\n## END : $SectionName\n" >> "$File"
	fi

	## Replace the section
	rep="## BEGIN : $SectionName\n$SectionData\n## END : $SectionName"
	awk -vrep="$rep" "/^## BEGIN : $SectionName/{print rep}/^## BEGIN : $SectionName/,/## END : $SectionName/{next}1" < "$File" > "$File".$$
	
	cat "$File.$$" > "$File"
	rm "$File.$$"
}

## Retrieves a section from a file
GetSection()
{
	local File="$1"
	local SectionName="$2"

	## Return an empty string if file does not exist
	if [[ ! -f "$File" ]]; then
		return 1
	fi

	local Delimiter1 Delimiter2
	Delimiter1="## BEGIN : $SectionName"
	Delimiter2="## END : $SectionName"
	awk "/^$Delimiter1/,/^$Delimiter2/ {print}" "$File" | grep -v "^$Delimiter1" | grep -v "^$Delimiter2"
}

## Clears and removes a section from a file
ClearSection()
{
	local File="$1"
	local SectionName="$2"
	local SectionData="$3"

	if [[ -f "$File" ]]; then
		PopulateSection "$File" "$SectionName" ""
	fi
}

## Checkes if a section exists in a file
CheckSectionExists()
{
	local File="$1"
	local SectionName="$2"

	ret=false
	if grep -q "^## BEGIN : $SectionName" "$File"; then
		if grep -q "^## END : $SectionName" "$File"; then
			ret=true
		fi
	fi

	echo $ret
}
