import QtQuick 2.0
Item{
    id:media_playback_base
    height: manager.appHeight
    width: manager.appWidth
    state:"metadata"

    Component.onCompleted: manager.setBoundStatus(true)
    property alias metadataComponent:mediaTypeMetaData.sourceComponent
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property alias controlComponent: controlsLoader.sourceComponent
    property alias playlistSource:playlist.model
    property alias playlistDelegate:playlist.delegate
    property bool enableScrollbar:true

    Item{
        id: options_display
    }

    Item{
        id:now_playing_image_container
    }

    Item{
        id:playlistPanel
        height: parent.height
        width: parent.width
        clip: true
        anchors.right: metaDataPanel.left
        PlaylistView{
            id:playlist
            anchors.left: parent.left
            delegate: Item{
                width: parent.width
                height: scaleY(12)
                Rectangle{
                    anchors.fill: parent
                    color: "black"
                    opacity: .65
                }
                StyledText{
                    text:name
                    color:"white"
                    fontSize: 36
                }
            }
        }
    }

    Item{
        id:metaDataPanel
        height: parent.height
        width: parent.width
        clip:true
        NowPlayingImage{
            anchors.centerIn: parent
        }

        Loader{
            id:mediaTypeMetaData
        }

        Loader{
            id:mediaScrollerTarget
        }

        Loader{
            id:controlsLoader
        }

        MouseArea{
            id:metadataDrag
            anchors.fill: parent
            onReleased: media_playback_base.state="playlist"
        }
    }


    states: [
        State {
            name: "metadata"
            AnchorChanges{
                target:metaDataPanel
                anchors.left: media_playback_base.left
            }
        },
        State {
            name: "playlist"
            AnchorChanges{
                target: metaDataPanel
                anchors.left: media_playback_base.right
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: 1000
                easing.type: Easing.InOutElastic
                easing.amplitude: .5
            }
        }
    ]

}
