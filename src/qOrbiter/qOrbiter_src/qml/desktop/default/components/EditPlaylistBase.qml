import QtQuick 1.1
import "../../../skins-common/lib/handlers"
Rectangle {
    id:editPlaylistBase
    color: "transparent"
    state: "hidden"
    anchors.centerIn: metadataSection
    onStateChanged:editView.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
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
    Connections{
        target: dcenowplaying
        onPlaylistPositionChanged:{
            editView.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
        }
    }

    ListView{
        id:editView
        height: parent.height - 50
        width: metadataSection.width/1.5
        clip: true
        flickableDirection: Flickable.VerticalFlick
       // model:manager.getDataGridModel("Playlist", 18)
        Component.onCompleted: currentIndex= dcenowplaying.m_iplaylistPosition
        highlightFollowsCurrentItem: true
        spacing:scaleY(1)
        Behavior on opacity {
            PropertyAnimation{
                duration: 750
            }
        }

        highlight: Rectangle{
            radius: 5
            color: "darkslategrey"
            opacity: .75

        }

        delegate:Rectangle{

            height: childrenRect.height
            width: parent.width
            color: index ==dcenowplaying.m_iplaylistPosition ? "grey": "darkslategrey"
            Row{
                height: children.height
                width: parent.width
                spacing:5

                Rectangle{
                    height: parent.height
                    width: parent.width*.08
                    color: "black"
                    StyledText {
                        text: index
                        anchors.centerIn: parent
                    }
                }

                StyledText {
                    text: name
                    width: parent.width*.70
                }
                Rectangle{
                    width: parent.width *.10
                    height: childrenRect.height
                    color: "darkslategrey"
                    StyledText{
                        text: "up"
                    }
                    PlaylistMoveHandler{
                        direction: "-"
                    }
                }
                Rectangle{
                    width: parent.width *.10
                    height: childrenRect.height
                    color: "darkslategrey"

                    StyledText{
                        text: "Down"

                    }
                    PlaylistMoveHandler{
                        direction: "+"
                    }
                }

                Rectangle{
                    width: parent.width *.10
                    height: childrenRect.height
                    color: "darkslategrey"
                    StyledText{
                        text: "Delete"
                    }
                    PlaylistRemoveItemHandler{

                    }
                }
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
            PropertyChanges {
                target: editView
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
            PropertyChanges {
                target: editView
                currentIndex: dcenowplaying.m_iplaylistPosition
                opacity:1

            }
        }
    ]
}
