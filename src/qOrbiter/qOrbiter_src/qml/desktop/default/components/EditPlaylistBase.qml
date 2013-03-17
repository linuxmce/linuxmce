import QtQuick 1.1

Rectangle {
    id:editPlaylistBase
    color: "transparent"
    state: "hidden"
    anchors.centerIn: metadataSection
    Behavior on height {
        PropertyAnimation{
            duration: 750
        }
    }

    Behavior on width{
        PropertyAnimation{
            duration: 750
        }
    }

    Behavior on opacity{
        PropertyAnimation{
            duration: 750
        }
    }

    Rectangle{
        id:mask
        anchors.fill: parent
        color: "black"
        opacity: .25
    }

    ListView{
        id:editView
        height: parent.height-50
        width: parent.width
        clip: true
        flickableDirection: Flickable.VerticalFlick
        model:mediaplaylist
        currentIndex: dcenowplaying.m_iplaylistPosition

        delegate: Row{
            height: children.height
            width: parent.width
            spacing:5
            StyledText {
                text: name + "::"+index
                width: parent.width*.50
            }
            StyledText{
                text: "up"
            }
            StyledText{
                text: "down"
            }
            StyledText{
                text: "Delete"
            }
        }
    }
    Row{
        id:playlistMeta
        width: editView.width
        height: 50
        anchors.top: editView.bottom
        anchors.left: editView.left
        StyledText{
            text: "Playlist Name: Foo"
            width: parent.width /2
        }
        StyledText{
            text: "Save"
        }

    }


    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: editPlaylistBase
                height:0
                width:0
                opacity:0

            }
        },
        State {
            name: "exposed"
            PropertyChanges {
                target: editPlaylistBase
                height: metadataSection.height
                width:metadataSection.width /2
                opacity:1
            }
        }
    ]
}
