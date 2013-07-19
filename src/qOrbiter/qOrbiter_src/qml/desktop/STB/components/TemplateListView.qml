import QtQuick 1.1
import "../../lib/handlers"
ListView{
    id:playlist
    height: scaleY(65)
    width: scaleX(38)
    anchors.left: parent.left
    spacing:scaleY(1)
    anchors.verticalCenter: parent.verticalCenter
    Component.onCompleted: manager.setBoundStatus(true)
    highlightFollowsCurrentItem: true
    model: dcenowplaying.qs_screen==="Screen_63.qml" ? simpleepg : mediaplaylist
    clip:true

    Connections{
        target: mediaplaylist
        onActiveItemChanged:{
            playlist.positionViewAtIndex(mediaplaylist.currentIndex, ListView.Beginning)
            playlist.currentIndex = mediaplaylist.currentIndex
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
            color:playlist.currentIndex === index? skinStyle.darkHighlightColor : skinStyle.mainColor
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
            fontSize:index === dcenowplaying.m_iplaylistPosition ? mediumText :mediumText
            color:"white"
            width: parent.width
            font.capitalization: Font.SmallCaps
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
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
