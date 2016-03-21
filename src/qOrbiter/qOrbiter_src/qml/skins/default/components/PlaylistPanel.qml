import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import "../."
Item{
    id:playlistPanel
    height: Style.scaleY(75)
    width: visible ? Style.listViewWidth_medium :0
    clip:true
    //  visible:showingPlaylist

    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:{
            console.log("Playlist position changed to "+dcenowplaying.m_iplaylistPosition)
            generic_model.listView.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
            generic_model.refresh()

        }
    }
    
    GenericListModel{
        id:generic_model
        label: qsTr("%1 item(s)").arg(modelCount)
        dataGrid:18
        dataGridLabel:"Playlist"
       // model:manager.getDataGridModel("Playlist", 18)
        delegate: Item{
            height:Style.scaleY(10)
            anchors{
                left:parent.left
                right:parent.right
            }
            clip:true
            anchors.margins: 5
            
            Rectangle{
                id:fil
                anchors.fill: parent
                color:ms.pressed ? Style.buttonPressedColor: "black"
                opacity:Style.appList_opacity
                
            }
            StyledText{
                text:description
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Style.fontSize_listItem
                width: parent.width
            }
            StyledText{
                text:value
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Style.fontSize_listItem
                opacity: value == dcenowplaying.m_iplaylistPosition ? 1 :.25
            }
            
            MouseArea{
                id:ms
                anchors.fill: parent
                onClicked: {manager.changedPlaylistPosition(index); console.log("Change track")}
            }
        }
        anchors.fill: parent
    }
}
