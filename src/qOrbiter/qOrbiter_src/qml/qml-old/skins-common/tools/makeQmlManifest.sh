#!/bin/bash
clear
#Work Area
CURRENT_DIRECTORY=""
CURRENT_SKIN_PATH=""
QML_ROOT=""
QMLDIR_HEADING="# This qmldir file has been automatically generated by makeQmlManifest.sh in qml_root/skins-common/tools. Use that tool to make sure you dont miss anything and please ensure you commit your changes"
target_paths=( "android" "desktop" "rpi" "qt5-desktop" "skins-common" "android/tablet" "android/phone" "android/tablet/qt5" "skins-common/qt5" )

function processSkinDirectory(){

echo "Processing $CURRENT_SKIN_PATH" 
cd $CURRENT_SKIN_PATH

if [ -e "qmldir" ]; then
echo "qmldir file exists, updating as needed"
rm qmldir
fi

touch qmldir
echo "$QMLDIR_HEADING" >> qmldir
echo "Added heading"

svn add qmldir
for skin in $CURRENT_SKIN_PATH/*
	do
	echo " Processing Item::"$skin

	if [ -f "$skin" ]; then
        currentFileName=$(basename "$skin")
 	currentFileType="${currentFileName##*.}"
	currentFullFileName="${currentFileName%.*}"
#	echo $currentFileType
#	echo $currentFileName

		if [ "$currentFileType" == "qml" ]; then
		echo "Adding $currentFileName to qmldir"
		QMLDIR_STRING="$currentFullFileName		$currentFileName"
		echo "$QMLDIR_STRING" >> qmldir		
		
		else
		echo $currentFileName " is Not a .qml file, skipping"
		fi
	else
	echo "$skin is Not file, needs inspection"
		
		if [ -d "$skin" ]; then
		echo "$skin is directory, processing"
		
		cd $skin
			if [ -e qmldir ];then
			rm qmldir
			fi
		touch qmldir
		svn add qmldir
		echo "$QMLDIR_HEADING" >> qmldir
			for subFile in "$skin"/*
			do
			currentSubFile=$(basename "$subFile")
			currentSubFileType="${currentSubFile##*.}"
        		currentSubFileName="${currentSubFile%.*}"
			#echo $currentSubFileType
        			if [[ -f "$subFile" && "$currentSubFileType" != "qmldir" ]]; then
	                	QMLDIR_STRING="$currentSubFileName             $currentSubFile                        #no comment"
                		echo "$QMLDIR_STRING" >> qmldir
#				echo $QMLDIR_STRING
                		else
                                echo " $subFile not a file but likely a skin, processing"
					if [ -d "$subFile" ]; then
                                                echo "$subFile is directory, processing"
                                                cd $subFile
							  if [ -e qmldir ];then
	        	               				 rm qmldir
                       						 fi
               			  		touch qmldir
                                                svn add qmldir --quiet
                                                svn add *.qml --quiet
            					 echo "$QMLDIR_HEADING" >> qmldir
                       			             for embeddedFile in "$subFile"/*
                       				      do
				                	 embeddedSubFile=$(basename "$embeddedFile")
                       				    	 embeddedSubFileType="${embeddedSubFile##*.}"
                      			             	 embeddedSubFileName="${embeddedSubFile%.*}"

                       				        	 if [[ -f "$embeddedFile" && "$embeddedSubFileType" != "qmldir" ]]; then
			                               		 QMLDIR_STRING="$embeddedSubFileName             $embeddedSubFile                        #no comment"
                        			       		 echo "$QMLDIR_STRING" >> qmldir
								#echo $QMLDIR_STRING
                               					# else
                               					# echo " $embeddedFile not a file but likely a skin, but this breaks organization and they will not be processed."
                                                                elif [ -d "$embeddedFile" ]; then
                                                               echo "Need to handle" "$embeddedFile"
  fi
							done
cd ..
					fi
                		fi
			done
			cd .. 
		fi
	fi 

done

#ls -lha
echo "                                       "
cd $QML_ROOT

}

function lookupDir(){

echo "Searching::$CURRENT_DIRECTORY"
#ls -lha $CURRENT_DIRECTORY

#now we go through and find skin directories

for currentDir in $CURRENT_DIRECTORY/*
do

	if [ -d "$currentDir" ]; then
	echo "$currentDir is to be processed"
	CURRENT_SKIN_PATH="$currentDir"
	processSkinDirectory
	else
	echo "$currentDir is a file and will be skipped"
	fi

done

echo "Done with $CURRENT_DIRECTORY"
echo "----------------------------
"

}

#----------
clear
echo "LinuxMCE QML manifest export - Automated qmldir creator"
echo "Switching to qml root"
cd ../../
QML_ROOT=$(pwd)
echo $QML_ROOT" is qml root dir"
echo "Beginning"
echo "Target Paths::" ${target_paths[@]}

for dir in "${target_paths[@]}"
do
TEST_PATH=$(pwd)"/""$dir"
echo $TEST_PATH
CURRENT_DIRECTORY=$TEST_PATH
lookupDir
done





