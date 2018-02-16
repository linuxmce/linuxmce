import QtQuick 2.2
import org.linuxmce.grids 1.0
import "../components"
import "../."

GenericListModel{
    id:playlistPanel
    label: modelCount + qsTr(" Channels.", "Tv Channels")+bcastSource
    property int itemCount:modelCount
    property int bcastSource:2
    dataGrid:DataGrids.EPG_All_Shows
    dataGridLabel:"channels"
    delegate: Item{
        height: appStyle.listViewItemHeight /2
        anchors{
            left:parent.left
            right:parent.right
        }
        clip:true
        Component.onCompleted: {
            if(index==0){
                playlistPanel.bcastSource=sourceid
            }
        }

        anchors.margins: 5

        //            Rectangle{
        //                anchors.fill: parent
        //                gradient:appStyle.buttonGradient
        //            }

        Rectangle{
            id:fil
            anchors.fill: parent
            color:ms.pressed ? appStyle.appbutton_confirm_color: "black"
            opacity:appStyle.appList_opacity

        }

        StyledText{
            text:name + "\n"+program
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: appStyle.fontSize_listItem
            width: parent.width
        }
        StyledText{
            text:channel
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: appStyle.fontSize_medium

            // opacity: value == dcenowplaying.m_iplaylistPosition ? 1 :.25
        }

        MouseArea{
            id:ms
            anchors.fill: parent
            onClicked: {manager.gridChangeChannel(channel, channelid)}
        }
    }
    width: appStyle.scaleX(35)
    
    //    Connections{
    //        target: dcenowplaying
    //        onPlayListPositionChanged:{
    //            console.log("Playlist position changed to "+dcenowplaying.m_iplaylistPosition)
    //            playlistView.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
    //            if(dcenowplaying.m_iplaylistPosition > playlistView.count-1){
    //                console.log("refresh playlist")
    //
    //                playlistView.model = manager.getDataGridModel("Channels", DataGrids.EPG_All_Shows)
    //            }
    //        }
    //    }
    

}
