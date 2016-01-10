import QtQuick 2.1
import "../../../skins-common/qt5-lib/handlers"
ListView{
    id:playlist
    width: scaleX(38)
    anchors{
        top:infoHdr.bottom
        bottom:parent.bottom
        margins: scaleX(1)
    }

    spacing:scaleX(1)
    Component.onCompleted: {manager.setBoundStatus(true);}
    highlightFollowsCurrentItem: true
   // model: dcenowplaying.qs_screen==="Screen_63.qml" ? manager.getDataGridModel("Channels", 11) : manager.getDataGridModel("Playlist", 18)
    clip:true


    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:{
            playlist_data.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
        }
    }

    Timer{
        id:indexUpdate
        interval: mediaplaylist.count()*10
        running: false
        onTriggered: {
            console.log("interval is" + interval)
            playlist.positionViewAtIndex(mediaplaylist.currentIndex, ListView.Beginning)
            console.log("Current index is " + mediaplaylist.currentIndex)
        }
    }
    
    delegate: Item{
        height: index === dcenowplaying.m_iplaylistPosition ? scaleY(15) : scaleY(10)
        width: parent.width - 10
        clip:true
        Keys.onPressed: {
            if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return){
                manager.changedPlaylistPosition(index)
            }
        }

        Rectangle{
            anchors.fill: parent
            color:playlist.currentIndex === index? "black" : appStyle.mainColor
            opacity: .85
            border.width: index === dcenowplaying.m_iplaylistPosition ? 5 : 2
            radius:5
            border.color: playlist.currentIndex === index? "white" : "black"
        }
        Image {
            id: playlistimage
            fillMode: Image.PreserveAspectCrop
            source:  index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
            anchors.fill: parent
            opacity: source !=="" ? .50 :0
        }

        StyledText{
            id:label
            text:/*index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + description : */program
            fontSize:index === dcenowplaying.m_iplaylistPosition ? headerText :mediumText
            color:"white"
            width: parent.width
            font.capitalization: Font.SmallCaps
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        GradientFiller{
            opacity: .50
            fillColor: "black"
        }

        PlaylistClickedHandler {
        }
    }
    
    states: [
        State {
            name: "hidden"
            AnchorChanges{
                target: playlist
                anchors.left: undefined
                anchors.right:template.left
            }
        }, State {
            name: "showing"
            AnchorChanges{
                target:playlist
                anchors.right: undefined
                anchors.left: template.left
            }
            StateChangeScript{
                script: {
                    playlist.forceActiveFocus();

                }
            }
        }
    ]
    
    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: appStyle.globalAnimationSpeed
                easing.type: appStyle.globalAnimationEasing
            }
        }
    ]
}
