#!/bin/bash

if [[ -n "$HEADER_AptSrc" ]]; then
	return 0
fi
HEADER_AptSrc=HEADER_AptSrc

# NOTE: This script doesn't check for validity of sources.list
# NOTE: Set AptSrc_Demo to a non-empty string to run the demo section

# Description: Add an apt source to memory
# TODO: use something faster than md5sum, but to the same effect: unique identifier for a string
AptSrc_AddSource()
{
	local Source="$*"
	local type uri distribution components
	local id
	local ret=1

	read type uri distribution components < <(echo "$Source")
	if [[ -z "$type" ]]; then
		return 1
	fi
#<-mkr_b_etch|via_b->
	# HACK: Don't add ubuntu sources to etch
	if [[ "$distribution" == feisty* || "$distribution" == "replacements_ubuntu" || "$distribution" == edgy* || "$distribution" == 20dev_ubuntu ]]; then
		return 1
	fi
#<-mkr_b_etch|via_e->

	uri="${uri%/}/" # make sure URI ends with a slash
	id=$(echo "$type $uri $distribution" | md5sum | cut -d' ' -f1)

	if [[ "$AptSrc_id_list" != *" $id "* ]]; then
		AptSrc_id_list="$AptSrc_id_list$id "
		eval "AptSrc_base_${id}='$type $uri $distribution'"
		eval "AptSrc_comp_${id}=' '" # AptSrc_comp_* has a leading space
		ret=0
	fi
	for comp in $components; do
		Var="AptSrc_comp_${id}"
		if [[ "${!Var}" == *" $comp "* ]]; then # AptSrc_comp_* is bound and separated by spaces
			continue
		fi
		eval "AptSrc_comp_${id}=\"\${AptSrc_comp_${id}}$comp \"" # AptSrc_comp_* has a trailing space
		ret=0
	done

	return $ret
}

# Description: Clear internal state
AptSrc_Clear()
{
	local Var
	for Var in ${!AptSrc_*}; do
		unset $Var
	done
}

# Description: Load a sources.list file
AptSrc_ParseSourcesList()
{
	local SourcesList="${1:-/etc/apt/sources.list}"
	local Source type uri distribution components
	local id comp Var

	AptSrc_Clear

	AptSrc_id_list=' '
	while read Source; do
		Source="${Source%%#*}" # remove comments
		AptSrc_AddSource "$Source" || :
	done < "$SourcesList"
	AptSrc_WasParsed=AptSrc_WasParsed
}

# Description: Output currently memorized sources.list
# Note: file: sources go first, before net sources
AptSrc_WriteSourcesList()
{
	local VarBase VarComp
	local id
	local -a file_source net_source
	local Source

	file_source=()
	net_source=()
	for id in $AptSrc_id_list; do
		VarBase="AptSrc_base_${id}"
		VarComp="AptSrc_comp_${id}"
		Source="${!VarBase} ${!VarComp}"
		read type uri distribution components < <(echo "$Source")
		if [[ "$uri" == file:* ]]; then
			file_source=("${file_source[@]}" "$Source")
		else
			net_source=("${net_source[@]}" "$Source")
		fi
	done
	for Source in "${file_source[@]}" "${net_source[@]}"; do
		echo "$Source"
	done
}
