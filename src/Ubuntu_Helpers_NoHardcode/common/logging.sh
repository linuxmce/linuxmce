#!/bin/bash

. /etc/lmce-build/builder.conf

exec 100>&1
exec >>"$log_file"
exec 2>&1

#set -x

function DisplayMessage {
	echo "`date +%H:%M:%S`  $*" >&100
}

function Error {
	set +x
	trap - EXIT

	echo
	DisplayMessage "ERROR: $*"
	echo				>&100
	echo 		 		>&100
	tail -c 10240 "$log_file"	>&100
	echo 				>&100

	# Mail the users
	mail_txt_file=$(mktemp)
	echo 							 >$mail_txt_file
	echo "Arch   : ${arch}"					>>$mail_txt_file
	echo "Flavor : ${flavor}"				>>$mail_txt_file
	echo "Date   : $(date -R)"				>>$mail_txt_file
	echo 							>>$mail_txt_file
	echo "Here are the last 10240 bytes of the log file:"	>>$mail_txt_file
	echo							>>$mail_txt_file
	tail -c 10240 "$log_file"				>>$mail_txt_file
	echo							>>$mail_txt_file

	cat $mail_txt_file | mail -s "$mail_subject_prefix Build Failure" "${mail_to}"

	rm -rf $mail_txt_file

	exit 1
}

function Warning {
	set +x

	echo
	DisplayMessage "WARNING: $*"
	echo				>&100
	echo 		 		>&100
	tail -c 10240 "$log_file"	>&100
	echo 				>&100

	# Mail the users
	mail_txt_file=$(mktemp)
	echo 							 >$mail_txt_file
	echo "Arch   : ${arch}"					>>$mail_txt_file
	echo "Flavor : ${flavor}"				>>$mail_txt_file
	echo "Date   : $(date -R)"				>>$mail_txt_file
	echo 							>>$mail_txt_file
	echo "BUILD WARNING:"					>>$mail_txt_file
	echo "$*"						>>$mail_txt_file
	echo							>>$mail_txt_file

	cat $mail_txt_file | mail -s "$mail_subject_prefix Build Warning" "${mail_to}"

	rm -rf $mail_txt_file

	set -x
}
