import QtQuick 2.2

ListView{
    id:playlist
    anchors{
        top:npHeader.bottom
        bottom:parent.bottom
        left:parent.left
    }
    width:parent.width *.25
    spacing:scaleY(1)

    Component.onCompleted: manager.setBoundStatus(true)
    highlightFollowsCurrentItem: true
    highlight: Rectangle{
        color:"red"
    }

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
        property bool isCurrent:index===dcenowplaying.m_iplaylistPosition
        height: isCurrent ? scaleY(15) : scaleY(10)
        width: parent.width - 10
        clip:true
        Keys.onPressed: {
            if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return){
                manager.changedPlaylistPosition(index)
            }
        }

        Rectangle{
            anchors.fill: parent
            color:isCurrent? skinstyle.darkHighlightColor : skinstyle.mainColor
            opacity: .85
            border.width: isCurrent ? 5 : 2
            radius:5
            border.color: isCurrent? "white" : "black"
        }
        Image {
            id: playlistimage
            fillMode: Image.PreserveAspectCrop
            source: isCurrent ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
            anchors.fill: parent
            opacity: 1
        }
        StyledText{
            id:label
            text:isCurrent ? "Now Playing - " + name : name
            fontSize:isCurrent ? mediumText :mediumText
            color:"white"
            width: parent.width
            font.capitalization: Font.SmallCaps
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        MouseArea{
            anchors.fill: parent
            onClicked: manager.changedPlaylistPosition(index)
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
