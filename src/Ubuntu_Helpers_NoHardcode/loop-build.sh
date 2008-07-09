#!/bin/bash -x
. /etc/lmce-build/builder.conf

while : ;do
	echo "$(date -R) Started" >> /var/log/loop-build.log	

	# Start Build
	BuildFinished="true"
	/usr/local/lmce-build/build.sh || BuildFinished="false"

	# Remove old Dir and keep last 5 build (hint: tail +5)
	for remove_dir in $(for dir in $(ls -r -d -X /home/ftp/AutoBuilds/build-*-${arch}); do echo $dir ;done | tail +5) ;do
		rm -rf "$remove_dir"
	done

	# Create Backup Dir
	build_ftp_dir="/home/ftp/AutoBuilds/build-$(date +%y_%m_%d_%s)-${arch}"
	mkdir -p "${build_ftp_dir}"

	# Backup the ISO
	mkdir -p "${build_ftp_dir}/ISO"
	cp /var/www/*.iso "${build_ftp_dir}/ISO/"
#	rm -f /var/www/*.iso

	# Cleanup the Build Dir of Private Sources
	rm -rf ${svn_dir}/${svn_branch_name}/src/ZWave
	rm -rf ${svn_dir}/${svn_branch_name}/src/Fiire_Scripts
	rm -rf ${svn_dir}/${svn_branch_name}/src/RFID_Interface

	# Backup the Build Dir
	svn_build_revision=$(svn info "$svn_dir/$svn_branch_name/src" | grep Revision | sed 's/Revision: //g')
	mkdir -p "${build_ftp_dir}/BUILD"
	cp -r ${svn_dir}/${svn_branch_name}/src/* "${build_ftp_dir}/BUILD"
	rm -rf ${svn_dir}/

	# Backup the Debs
	mkdir -p "${build_ftp_dir}/DEBS"
	cp -r /var/www/*.deb "${build_ftp_dir}/DEBS"
	rm -f /var/www/*.deb
	
	# Backup the log
	mkdir -p "${build_ftp_dir}/LOGS"
	cp /var/log/lmce-build.log "${build_ftp_dir}/LOGS" || :
	mv /var/log/mce-installer-*.log "${build_ftp_dir}/LOGS" || :
	mv /var/log/Config_Device_Changes.log "${build_ftp_dir}/LOGS" || :
	
	echo "$(date -R) Done" >> /var/log/loop-build.log

	if [[ "$BuildFinished" == "true" ]] ;then
		# Store the last known good svn revision
		svn_prev_revision=$(cat "$build_dir/svn_last_good")
		echo "$svn_build_revision" > "$build_dir/svn_last_good"

		# Send mail informing that the build had finished
		mail_txt_file=$(mktemp)
		echo                                                     >$mail_txt_file
		echo "Arch     : ${arch}"                               >>$mail_txt_file
		echo "Flavor   : ${flavor}"                             >>$mail_txt_file
		echo "Revision : ${svn_build_revision}"			>>$mail_txt_file
		echo "Date     : $(date -R)"                            >>$mail_txt_file
		echo "MD-32    : $(cat $build_dir/svn_i386)"            >>$mail_txt_file
		echo "MD-64    : $(cat $build_dir/svn_amd64)"           >>$mail_txt_file
		echo "URL      : ftp://builder32.linuxmce.com/AutoBuilds/$(basename $build_ftp_dir)" >>$mail_txt_file
		echo							>>$mail_txt_file
		echo							>>$mail_txt_file
		if [[ "$svn_prev_revision" != "" ]] ;then
			echo " * Changes since the last build : *"		>>$mail_txt_file	
			echo							>>$mail_txt_file
			svn log --xml -v -r$svn_build_revision:$svn_prev_revision $svn_url | xsltproc /etc/svn2cl/svn2cl.xsl - >>$mail_txt_file
		fi
		cat $mail_txt_file | mail -s "$mail_subject_prefix Build Completed" "${mail_to}"
		rm -rf $mail_txt_file
	fi
done 
