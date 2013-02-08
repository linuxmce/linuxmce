#!/bin/bash
. /usr/pluto/bin/TeeMyOutput.sh --outfile /var/log/pluto/LMCEUpdate.log --stdboth --append -- "$@"
set -e

UPDATES_XML_URI="http://10.0.0.83/updates.xml"
UPDATES_DIR="/home/updates"
POUT=101
PIN=100

## Usefull functions
function Debug {
	echo "$(date -R) $*" >&2
	echo "$(date -R) $*" >> /var/log/pluto/UpdateHelper.log
}

function Receive {
	local msg
	read msg <&$PIN
	Debug "RECV : $msg"
	echo "$msg"
}

function Send {
	local msg="$*"
	Debug "SENT : $msg"
	echo "$msg" >&$POUT
}

function _Param {
	local param_no="$1"
	shift
	echo ${!param_no}
}

function Param {
	local param_no="$1"
	local line="$2"

	eval _Param $param_no $line
}

function GetValue {
	local list="$2"
	local name="$1"

	# remove the spaces from in front/after the = sign
	list=$(echo $list | sed 's/ *= *"/="/g')

	echo $(eval "$list"; echo ${!name})
}


## Protocol functions
function Download {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
		return
	fi

	local update_url="$2"
	if [[ "$update_url" == "" ]] ;then
		Send "FAIL Url is empty"
		return
	fi

	local update_md5="$3"
	if [[ "$update_md5"  == "" ]] ;then
		Send "FAIL MD5 is missing"
		return
	fi

	local update_file=$(basename "$update_url")
	if [[ "$update_file" == "" ]] ;then
		Send "FAIL Cannot extract filename from url"
		return
	fi

	mkdir -p "${UPDATES_DIR}/${update_no}"
	
	wget -c --tries=5 --timeout=5 -O "${UPDATES_DIR}/${update_no}/${update_file}" "${update_url}" || :

	if [[ ! -f "${UPDATES_DIR}/${update_no}/${update_file}" ]] ;then
		Send "FAIL No file can be downloaded"
		return
	fi

	if [[ "$(md5sum "${UPDATES_DIR}/${update_no}/${update_file}" | cut -d' ' -f1)" != "${update_md5}" ]] ;then
		Send "FAIL MD5 sum does not match"
		return
	fi

	Send "OK"
}

function ValidateUpdate {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
		return
	fi

	local update_xml="$2"
	if [[ ! -f "${update_xml}" ]] ;then
		Send "FAIL Update xml file is not set"
		return
	fi

	cp "$update_xml" "${UPDATES_DIR}/${update_no}/update.xml"
	Send "OK"
}

function CheckUpdate {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
		return
	fi

	if [[ -f "${UPDATES_DIR}/${update_no}/update.xml" ]] ;then
		Send "OK"
	else
		Send "FAIL Update is not downloaded" 
	fi
}

function UpdateXml {
:	
}

function Apply {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
		return
	fi

	local update_url="$2"
	if [[ "$update_url" == "" ]] ;then
		Send "FAIL Url is empty"
		return
	fi

	local update_file=$(basename "$update_url")
	if [[ "$update_file" == "" ]] ;then
		Send "FAIL Cannot extract filename from url"
		return
	fi

	if [[ ! -f "${UPDATES_DIR}/${update_no}/${update_file}" ]] ;then
		Send "FAIL Cannot find update file"
		return
	fi
	
	local update_values="$3"
	if [[ "$update_values" == "" ]];then
		Send "FAIL Can't find information on how to apply update"
		return
	fi

	case "$(echo `GetValue "action" "$update_values"` | tr "[:lower:]" "[:upper:]")" in
		"DEB")
			ApplyDeb "${UPDATES_DIR}/${update_no}/${update_file}" "${update_values}" || return
		;;
		"UNTAR")
			ApplyUntar "${UPDATES_DIR}/${update_no}/${update_file}" "${update_values}" || return
		;;
		"RUN")
			ApplyRun "${UPDATES_DIR}/${update_no}/${update_file}" "${update_values}" || return
		;;
	esac

	Send "OK"
}

function ApplyDeb {
	local file="$1"
	local values="$2"
	
	if  ! dpkg -i "$file" ;then
		Send "FAIL Cannot install deb \"$file\""
		return 1
	fi

	return 0
}

function ApplyRun {
	local file="$1"
	local values="$2"

	if [[ ! -f "$file" ]] ;then
		Send "FAIL Cannot find file to be executed '$file'"
		return 1
	fi

	chmod +x "$file" 

	if ! "$file" ;then
		Send "FAIL Fail to execute file '$file'"
		return 1
	fi

	return 0
}

function ApplyUntar {
	local file="$1"
	local values="$2"

	local destination=$(GetValue "destination" "$values")
	if [[ "$destination" == "" ]] ;then
		destination="/"
	fi

	if [[ ! -d "$destination" ]] ;then
		if ! mkdir -p "$destination" ;then
			Send "FAIL Cannot create untar destination '$destination'"
			return 1
		fi
	fi

	if [[ "$file" = *.tar.gz || "$file" = *.tgz ]] ;then
		if ! tar -C "$destination" -zxf "$file" ;then
			Send "FAIL Cannot untar file '$file'"
			return 1
		fi

	elif [[ "$file" = *tar.bz2 || "$file" = *tbz2 ]] ;then
		if ! tar -C "$destination" -jxf "$file" ;then
			Send "FAIL Cannot untar file '$file'"
			return 1
		fi

	elif [[  "$file" = *tar ]] ;then
		if ! tar -C "$destination" -xf "$file" ;then
			Send "FAIL Cannot untar file '$file'"
			return 1
		fi
	else
		Send "FAIL Unrecognized file format '$file'"
		return 1
	fi

	local autoexec=$(GetValue "autoexec" "$values")
	if [[ "$autoexec" != "" ]] ;then
		if [[ ! -x "$autoexec" ]] ;then
			Send "FAIL Cannot find auntoexec script '$autoexec'"
			return 1
		fi

		if ! ${autoexec} ;then
			Send "FAIL Autoexec script '$autoexec' exited with an error"
			return 1
		fi
	fi

	return 0
}

function Apply_Option {
	echo "** APPLY_OPTION $*"

	local update_no="$1"
	local option="$2"


	if [[ "$option" == "reboot" ]] ;then
		Debug "REBOOT REQUESTED"
		Debug "PERFORMING REBOOT"
		reboot &
		sleep 60
		exit 
#		Send "OK"
		return 0
	fi

	if [[ "$option" == "reload" ]] ;then
		Debug "***** RELOADING THE ROUTER *******"
		Send "OK"
		return 0
	fi

	Send "FAIL Unknown option ($option)"
	return 1
}

## Message loop
Debug "Helper Script Started"
trap 'Debug "Helper Script End"' EXIT
while /bin/true ;do
	line=$(Receive)
	if [[ "$line" == "" ]] ;then
		exit
	fi
	command=$(Param 1 "$line")

	case "${command}" in
		"UPDATE_XML")
			# UPDATE_XML
			UpdateXml
			;;
		"DOWNLOAD")
			# DOWNLOAD <UPDATE_ID> <URL> <MD5>
			Download "$(Param 2 "$line")" "$(Param 3 "$line")" "$(Param 4 "$line")"
			;;
		"VALIDATE_UPDATE")
			# VALIDATE_UPDATE <UPDATE_ID> <XML_FILE>
			ValidateUpdate "$(Param 2 "$line")" "$(Param 3 "$line")"
			;;
		"CHECK_UPDATE")
			# CHECK_UPDATE <UPDATE_ID>
			CheckUpdate "$(Param 2 "$line")"
			;;
		"APPLY")
			# APPLY <UPDATE_ID> <URL> [param1="value1" param2="value2" ... paramN="valueN"]
			Apply "$(Param 2 "$line")" "$(Param 3 "$line")" "$(echo $line | cut -d' ' -f4-999)"
			;;
		"APPLY_OPTION")
			# APPLY_OPTION
			Apply_Option "$(Param 2 "$line")" "$(Param 3 "$line")"
			;;
		"EXIT")
			# EXIT
				exit
			;;
		*)
			Debug "FAIL Unkown Command"
			;;
	esac
done
