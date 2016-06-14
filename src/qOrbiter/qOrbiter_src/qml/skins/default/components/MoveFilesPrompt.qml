import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.grids 1.0
import "../components"
import "../."

GenericPopup{
    id:moveFilesToBox
    anchors.fill: parent
    title:"Select location to move file to"
    content:Item{
        id:move_content

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
            dataGridLabel: "Directories"
            dataGridOptions: move_content.getStartingDirectory();
            label: "Directories"
            anchors.fill: parent
            delegate: Rectangle{
                width: parent.width
                height: 60
                color:"black"
                StyledText{
                    text:value+"\n"+description
                    color:"red"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {dir_listing.dataGridOptions=value; dir_listing.refresh() }
                }
            }
        }
    }
}
