#!/bin/bash
#
# LinuxMCE script
# This script shows/adds/deletes local radio stations that it finds from the website http://www.radiotime.com
# to/from the local media database.
#
#Var Declaraions
if [[ "$2" == "" ]]; then
        DownloadURL="http://opml.radiotime.com/Browse.ashx?c=local&filter=s:popular"
else
        DownloadURL="$2"
fi

NameOfTempFile=LocalRadio.Temp

case $1 in
        show)
                #Download All Local Radio Stations Index
                wget -q --output-document=$NameOfTempFile $DownloadURL

                #Process Index
                cat $NameOfTempFile | grep audio | grep station | while read LINE ; do

                        #Get Station Name
                        Station=$(echo $LINE | gawk -F'"' '{ print $4}' | gawk -F'(' '{print $1}' | gawk -F"|" '{print $1 "-" $2}')
                        echo -e "Radio Station  : \e[01;33m$Station\e[00m"

                        #Get Url
                        echo $LINE | gawk -F'&amp;f' '{print $1}' | gawk -F'"' '{system("curl --silent "$6"")}' | while read Url ; do
                                echo -e "URL            : $Url"
                        done

                        #Get Genre
                        Genre=$(echo $LINE | gawk -F'"' '{ print $4}' | gawk -F'(' '{print $2}' | gawk -F')' '{print $1}')
                        echo  -e "Genre         : $Genre"

			#Get Picture
			Picture=$(echo $LINE | gawk -F image '{print $2}' | gawk -F'"' '{ print $2}')
                        echo -e "Picture            : $Picture"

                        echo
                done

                #Remove temp file
                rm $NameOfTempFile
        ;;

        add)
                #Download All Local Radio Stations Index
                wget -q --output-document=$NameOfTempFile $DownloadURL

                #Process Index
                cat $NameOfTempFile | grep audio | grep station | while read LINE ; do

                        #Get Station Name
                        Station=$(echo $LINE | gawk -F'"' '{ print $4}' | gawk -F'(' '{print $1}' | gawk -F"|" '{print $1 "-" $2}')

                        #Display what we are doing
                        echo -e "Adding Radio Station : \e[01;33m$Station\e[00m To LinuxMCE"

                        #Get Url
                        Url=$(echo $LINE | gawk -F'&amp;f' '{print $1}' | gawk -F'"' '{system("curl --silent "$6" | head -n1")}')

			#Check for [playlist] in stream URL (Needed for some stations)
			Url2=$(curl --silent -m 1 "$Url" | grep -A 3 "\[playlist\]" | grep "File1" | gawk -F'=' '{ print $2 }')
			if [[ $Url2 ]]; then
				Url=$Url2
			fi

			#Check for m3u playlist (Needed for some stations)
			if [[ "$Url" == *m3u ]]; then
				Url=$(wget -qO- "$Url" | grep http | tail -n 1)
			fi

			#Remove nonprintable characters from Url
			Url=$(tr -dc '[[:print:]]' <<< "$Url")

			#Strip URL of session information
			Url=$(echo "$Url" | gawk -F'?' '{ print $1}')

                        #Get Genre
                        Genre=$(echo $LINE | gawk -F'"' '{ print $4}' | gawk -F'(' '{print $2}' | gawk -F')' '{print $1}')

			#Get Picture
			Picture=$(echo $LINE | gawk -F" image" '{print $2}' | gawk -F'"' '{ print $2}')

                        #Check if Url is already in Database
       	                if ! [ $(mysql pluto_media -ss -e "select PK_File from File where Filename='$Url';") ]; then

				#Check if stream is Active
				curl --silent -m 1 "$Url" > /dev/null
				Error=$?
				if [[ $Error -eq "28" ]]; then

	                                #Add Url to Database
        	                        PK_File=$(mysql pluto_media -ss -e "insert into File (EK_MediaType,DateAdded,Filename,Missing,IsDirectory,IsNew) VALUES(43,NOW(),'$Url',0,0,1);select LAST_INSERT_ID() from File;" | tail -n1)

                	                #Add Station Name to Database
                        	        FK_Attribute=$(mysql pluto_media -ss -e "insert into Attribute (FK_AttributeType,Name) VALUES(10,'$Station');select LAST_INSERT_ID() from Attribute;" | tail -n1)

                                	#Hook Station Name to Url
	                                mysql pluto_media -e "insert into File_Attribute (FK_File,FK_Attribute,Track,Section) VALUES('$PK_File','$FK_Attribute',0,0);"

        	                        #Check If Genre Exists in Database
                	                GenreID=$(mysql pluto_media -ss -e "select PK_Attribute from Attribute where FK_AttributeType=8 and Name='$Genre';"| tail -n1)
                        	        if ! [ $(echo $GenreID) ]; then
                                	        #Add Genre to Database
                                        	GenreID=$(mysql pluto_media -ss -e "insert into Attribute (FK_AttributeType,Name) VALUES(8,'$Genre');select LAST_INSERT_ID() from Attribute;" | tail -n1)
	                                fi

        	                        #Hook Genre to Url
                	                mysql pluto_media -e "insert into File_Attribute (FK_File,FK_Attribute,Track,Section) VALUES('$PK_File','$GenreID',0,0);"

					#Download Picture
		                	wget -q --output-document=TempPic.png $Picture
					#Convert Picture to jpg
					mogrify -format jpg TempPic.png
					#Insert Placeholder for Picture
					PK_Picture=$(mysql pluto_media -e "insert into Picture (Extension,URL) VALUES('jpg','$Picture');select LAST_INSERT_ID() from Attribute;" | tail -n1)
					#Move Picture to correct location
					mv TempPic.jpg /home/mediapics/$PK_Picture.jpg
					#Thumbnailify the Picture
					convert -thumbnail 256 /home/mediapics/"$PK_Picture".jpg /home/mediapics/"$PK_Picture"_tn.jpg
					#Hook Station Name to Picture
                                	mysql pluto_media -e "insert into Picture_Attribute (FK_Picture,FK_Attribute) VALUES('$PK_Picture','$FK_Attribute');"


				else
        		        echo -e "\e[00;31mError\e[00m : Stream not Active"
                		fi
			else
        	        echo -e "\e[00;31mError\e[00m : URL Already in Database"
                	fi



			if [ -f $TempPic.png ];
			then
			rm TempPic.png
			fi
                done

                #Remove temp file
                rm $NameOfTempFile
        ;;

        delete)
                #Download All Local Radio Stations Index
                wget -q --output-document=$NameOfTempFile $DownloadURL

                #Process Index
                cat $NameOfTempFile | grep audio | grep station | while read LINE ; do
                        #Get Station Name

                        #Display What we are doing
                        echo -e "Deleting Radio Station : \e[01;33m$Station\e[00m From LinuxMCE"

                        #Get Genre
                        Genre=$(echo $LINE | gawk -F'"' '{ print $4}' | gawk -F'(' '{print $2}' | gawk -F')' '{print $1}')

                        #Get Url
                        echo "$LINE" | gawk -F'&amp;f' '{print $1}' | gawk -F'"' '{system("curl --silent "$6"")}' | while read Url ; do

			#Check for [playlist] in stream URL (Needed for some stations)
			Url2=$(curl --silent -m 1 "$Url" | grep -A 3 "\[playlist\]" | grep "File1" | gawk -F'=' '{ print $2 }')
			if [[ $Url2 ]]; then
				Url=$Url2
			fi

			#Check for m3u playlist (Needed for some stations)
			if [[ "$Url" == *m3u ]]; then
				Url=$(wget -qO- "$Url" | grep http | tail -n 1)
			fi


                                #Check if Url is in Database
                                if [ $(mysql pluto_media -ss -e "select PK_File from File where Filename='$Url';" | tail -n1) ]; then

                                        #Delete Url from Database
                                        PK_File=$(mysql pluto_media -ss -e "select PK_File from File where Filename='$Url';" | tail -n1)
                                        mysql pluto_media -e "delete from File where PK_File=$PK_File;"

                                        #Delete Station Name
                                        PK_Attribute=$(mysql pluto_media -ss -e "select PK_Attribute from Attribute where Name='$Station';")
                                        mysql pluto_media -e "delete from Attribute where PK_Attribute=$PK_Attribute;"

                                        #Delete Hook for Station Name
                                        mysql pluto_media -e "delete from File_Attribute where FK_File="$PK_File" and FK_Attribute="$PK_Attribute";"

                                        #Delete Hook for Genre
                                        PK_Attribute=$(mysql pluto_media -ss -e "select PK_Attribute from Attribute where Name='$Genre';")
                                        mysql pluto_media -e "delete from File_Attribute where FK_File=$PK_File and FK_Attribute=$PK_Attribute;"

                                else
                                        echo -e "\e[00;31mError\e[00m : URL not in Database"
                                fi
                        done
                done

                #Remove temp file
#               rm $NameOfTempFile
        ;;

        *)
                echo "usage: $0 {show|add|delete}"
                exit 1
        ;;
esac
exit 0

