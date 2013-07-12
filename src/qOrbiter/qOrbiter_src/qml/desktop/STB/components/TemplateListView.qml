import QtQuick 1.1
import "../../lib/handlers"
ListView{
    id:playlist
    height: scaleY(65)
    width: scaleX(35)
    anchors.left: parent.left
    anchors.verticalCenter: parent.verticalCenter
     Component.onCompleted: manager.setBoundStatus(true)
    model: dcenowplaying.qs_screen==="Screen_63.qml" ? simpleepg : mediaplaylist
    
    Connections{
        target: mediaplaylist
        onActiveItemChanged:{
            playlist.positionViewAtIndex(mediaplaylist.currentIndex, ListView.Beginning)
        }
    }
    
    delegate: Item{
        height: index === dcenowplaying.m_iplaylistPosition ? scaleY(12) : scaleY(8)
        width: parent.width
        clip:true
        Rectangle{
            anchors.fill: parent
            color:playlist.currentIndex === index? skinStyle.darkHighlightColor : skinStyle.lightHighlightColor
            opacity: .85
            border.width: index === dcenowplaying.m_iplaylistPosition ? 4 : 0
        }
        Image {
            id: playlistimage
            fillMode: Image.PreserveAspectCrop
            source:  index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
            anchors.fill: parent
            opacity: 1

            Connections{
                target: mediaplaylist
                onActiveItemChanged: indexUpdate.start()
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

        }
        StyledText{
            id:label
            text:index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + name : name
            fontSize: mediumText
            color:"white"
            width: parent.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
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
                    playlist.forceActiveFocus()
                }
            }
        }
    ]
    
    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: 750
            }
        }
    ]
}
