import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.grids 1.0
import "../components"
import "../."

GenericPopup{
    id:moveFilesToBox
    anchors.fill: parent
    title:qsTr("Select Directory. Tap  name to navigate down")
    content:Item{
        id:move_content

        function moveUp(){
            dir_listing.currentLocation = getStartingDirectory()
            dir_listing.dataGridOptions=(getStartingDirectory()+"\n~MT");
            dir_listing.refresh()
        }

        function getStartingDirectory(){
            var ret="/"
            var t = Number(manager.q_mediaType)
            switch(t){
            case MediaTypes.LMCE_Pictures :
            case MediaTypes.NP_Picture:
            case MediaTypes.LMCE_AirPlay_photos :ret= "/home/public/data/pictures/";break;
            case MediaTypes.LMCE_AirPlay_video:
            case MediaTypes.LMCE_BD:
            case MediaTypes.LMCE_DVD:
            case MediaTypes.LMCE_HDDVD:
            case 5:
            case MediaTypes.LMCE_StoredVideo:  ret = "/home/public/data/videos/";break;
            case MediaTypes.LMCE_Airplay_audio:
            case MediaTypes.LMCE_StoredAudio:
            case 4:
            case MediaTypes.LMCE_StreamedAudio: ret = "/home/public/data/audio/";break;

            default: ret="/home/public/data/"; break;
            }
            ret+="\n~MT"
            console.log( "Returning starting directory for mediatype ->"+ String(manager.q_mediaType) + ret)
            return ret;
        }

        Rectangle{
            anchors.fill: parent
            color:"grey"
        }
        
        GenericListModel{
            id:dir_listing
            dataGrid: DataGrids.Directory_Listing
            dataGridLabel: "directory_listing"
            dataGridOptions: move_content.getStartingDirectory();
            label: qsTr("Current Directory: %1").arg(currentLocation)
            property string currentLocation:move_content.getStartingDirectory()
            anchors{
                left:parent.left
                right:parent.right
                bottom:dir_up.top
                top:parent.top
            }

            delegate: Rectangle{
                width: parent.width
                height: appStyle.listViewItemHeight
                color:"black"


                StyledText{
                    text:qsTr("Directory: ")+value
                    color:"white"
                    anchors.topMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {dir_listing.currentLocation=value ;dir_listing.dataGridOptions=(value+"\n~MT"); dir_listing.refresh(); }
                }

                StyledButton{
                    buttonText: qsTr("Select this directory")
                    height: parent.height/2
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    width: appStyle.appButtonWidth
                    onActivated: {
                        var target = filedetailsclass.path+filedetailsclass.file
                        console.log("moving file "+ target)
                        console.log("to "+value)
                        mediaHelper.moveFile(target, value)
                    }
                }

            }
        }
        LargeStyledButton{
            id:this_dir
           anchors.centerIn: dir_listing
           width: appStyle.appButtonLargeWidth
            height: appStyle.appButtonHeight
            buttonText: qsTr("Select this directory.")
            onActivated: {
                var target = filedetailsclass.path+filedetailsclass.file
                console.log("moving file "+ target)
                console.log("to "+dir_listing.currentLocation )
                mediaHelper.moveFile(target, dir_listing.currentLocation)
            }
            visible: dir_listing.modelCount===0
        }

        LargeStyledButton{
            id:dir_up
            anchors{
                left:parent.left
                right:parent.right
                bottom:parent.bottom
            }
            height: appStyle.appButtonHeight
            buttonText: qsTr("Move up one level")
            onActivated:move_content.moveUp()
        }
    }
}
