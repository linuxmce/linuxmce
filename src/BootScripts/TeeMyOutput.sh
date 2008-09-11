#!/bin/bash

# Usage:
# source TeeMyOutput.sh --outfile <file> [--infile <file>] [--stdout|--stderr|--stdboth] [--append] [--exclude <egrep pattern>] -- "$@"
#   --outfile <file>         the file to tee our output to
#   --infile <file>          the file to feed ourselves with on stdin
#   --stdout                 redirect stdout (default)
#   --stderr                 redirect stderr
#   --stdboth                redirect both stdout and stderr
#   --append                 run tee in append mode
#   --exclude <pattern>      strip matching lines from output; pattern is used with egrep
#
# Environment:
#   SHELLCMD="<shell command>" (ex: bash -x)

if [[ -n "$TeeMyOutput" ]]; then
	return 0
fi
Me="TeeMyOutput"

# parse parameters
for ((i = 1; i <= "$#"; i++)); do
	Parm="${!i}"
	case "$Parm" in
		--outfile) ((i++)); OutFile="${!i}" ;;
		--infile) ((i++)); InFile="${!i}" ;;
		--stdout|--stderr|--stdboth) Mode="${!i#--std}" ;;
		--append) Append=yes ;;
		--exclude) ((i++)); Exclude="${!i}" ;;
		--) LastParm="$i"; break ;;
		*) echo "$Me: Unknown parameter '$Parm'"; exit 1
	esac
done

if [[ -z "$OutFile" ]]; then
	echo "$Me: No outfile"
	exit 1
fi

if [[ -z "$LastParm" ]]; then
	LastParm="$#"
fi

# original parameters
for ((i = "$LastParm" + 1; i <= "$#"; i++)); do
	OrigParms=("${OrigParms[@]}" "${!i}")
done

# construct command components
case "$Mode" in
	out) OurRedirect=() ;;
	err) OurRedirect=("2>&1" "1>/dev/null") ;;
	both) OurRedirect=("2>&1") ;;
esac

if [[ "$Append" == yes ]]; then
	TeeParm=(-a)
fi

if [[ -n "$InFile" ]]; then
	OurRedirect=("${OurRedirect[@]}" "<$InFile")
fi

# do our stuff
export TeeMyOutput=yes
ExitCodeFile="/tmp/TeeMyOutputExitCode_$$"
trap "rm -rf '$ExitCodeFile'" EXIT

Run()
{
	eval exec "${OurRedirect[@]}"
	$SHELLCMD "$0" "${OrigParms[@]}"
	echo $? >"$ExitCodeFile"
}

if [[ -z "$Exclude" ]]; then
	Run | tee "${TeeParm[@]}" "$OutFile"
else
	Run | grep --line-buffered -v "$Exclude" | tee "${TeeParm[@]}" "$OutFile"
fi

ExitCode=$(<"$ExitCodeFile")
exit "$ExitCode"
exit 1 # just in case
