import QtQuick 1.1
import "../components"
/*!
  This is screen 54, the stored audio remote screen.
  It has
  *Playlist
    -Playlist Manipulation controls.
  *Media Playback controls
  *Media Image
  *Lighting Control
  *Audio Control
  *Advanced Device Controls
  
  */
Panel{
    id:playlistPanel
    headerTitle: qsTr("Playlist ") + playlistView.count + qsTr(" Items.")
    property int itemCount:playlistView.count
    anchors{
        left:parent.left
        top:parent.top
        bottom:parent.bottom
    }
    width: scaleX(35)
    
    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:{
            console.log("Playlist position changed to "+dcenowplaying.m_iplaylistPosition)
            playlistView.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
            if(dcenowplaying.m_iplaylistPosition > playlistView.count-1){
                console.log("refresh playlist")
                manager.clearDataGrid("Playlist")
                playlistView.model = manager.getDataGridModel("Playlist", 18)
            }
        }
    }
    
    ListView{
        id:playlistView
        clip:true
        anchors{
            top:playlistPanel.headerRect.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        spacing: 5
        model:manager.getDataGridModel("Playlist", 18)
        onCountChanged: positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

        
        delegate: Item{
            height: manager.appHeight*.10
            anchors{
                left:parent.left
                right:parent.right
            }
            clip:true

            anchors.margins: 5
            
//            Rectangle{
//                anchors.fill: parent
//                gradient:style.buttonGradient
//            }
            
            Rectangle{
                id:fil
                anchors.fill: parent
                color:ms.pressed ? style.buttonPressedColor: "black"
                opacity:style.buttonOpacity
                
            }
            
            StyledText{
                text:description
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 22
                isBold: true
                width: parent.width
            }
            StyledText{
                text:value
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 36
                isBold: true
                opacity: value == dcenowplaying.m_iplaylistPosition ? 1 :.25
            }
            
            MouseArea{
                id:ms
                anchors.fill: parent
                onClicked: {manager.changedPlaylistPosition(index); console.log("Change track")}
            }
        }
        
    }
    states:[
            State{
                name:"playlistopen"
                when:showingPlaylist
                AnchorChanges{
                    target: playlistPanel
                    anchors{
                        right:undefined
                        left:media_playback_base.left
                    }
                }
            },
            State{
                name:"playlistclosed"
                when:!showingPlaylist
                AnchorChanges{
                    target: playlistPanel
                    anchors{
                        left:undefined
                        right:media_playback_base.left
                    }
                }
            }
    ]


    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration:skinstyle.animation_medium
                easing.type: skinstyle.animation_easing


            }
        }
    ]
}
