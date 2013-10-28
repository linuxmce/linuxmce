import QtQuick 1.0
Item{
    id:media_playback_base
    height: parent.height
    width: manager.appWidth
    state:"metadata"

    Component.onCompleted: {

        nav_row.navSource="NavOptions5.qml";
        info_panel.state="hidden";
        controlComponent = "Controls_5.qml" //controlsLoader.sourceComponent
    }


    property string metadataComponent:"Metadata_"+manager.i_current_mediaType+".qml"
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property string controlComponent: ""
    property Item playlistSource
    property Item playlistDelegate
    property bool enableScrollbar:true
    property alias playListComponent:playlist.sourceComponent

    Item{
        id: options_display
    }

    Item{
        id:now_playing_image_container
    }

    AdvancedMediaOptions {
        id: controlPanel
    }


    Item{
        id:metaDataPanel
        height: parent.height
        width: parent.width
        clip:true

        NowPlayingImage{
            id:npImage
            anchors.top:parent.top
        }

        Loader{
            id:mediaTypeMetaData
            width: parent.width-npImage.width
            source: metadataComponent
            anchors.right: metaDataPanel.right
            anchors.top: npImage.top
        }

        Loader{
            id:mediaScrollerTarget
        }

        Loader{
            id:controlsLoader
            width: parent.width

            source:controlComponent
            anchors.bottom: parent.bottom
        }
        Item{
            id:hiddenDrag
            height: parent.height
            width: parent.width/2
            x:parent.width /2
        }
    }

    Item{
        id:playlistPanel
        height: parent.height
        width: parent.width
        clip: true
        anchors.left: metaDataPanel.right

        Loader{
            id:playlist
            sourceComponent: PlaylistView{}
        }

    }

    states: [
        State {
            name: "metadata"
            AnchorChanges{
                target:metaDataPanel
                anchors.right: undefined
                anchors.left: media_playback_base.left
            }
            PropertyChanges{
                target:controlPanel
                state:"unloaded"
            }

            PropertyChanges {
                target: hiddenDrag
                x:media_playback_base.x / 2
            }
        },
        State {
            name: "playlist"
            AnchorChanges{
                target: metaDataPanel
                anchors.left: undefined
                anchors.right:media_playback_base.left
            }
            PropertyChanges{
                target:controlPanel
                state:"unloaded"
            }

        },
        State {
            name: "controls"
            AnchorChanges{
                target: metaDataPanel
                anchors.right: undefined
                anchors.left: media_playback_base.right
            }
            PropertyChanges{
                target:controlPanel
                state:"loaded"
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
