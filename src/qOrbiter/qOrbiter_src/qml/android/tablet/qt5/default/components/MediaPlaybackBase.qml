import QtQuick 2.0
Item{
    id:media_playback_base
    height: parent.height
    width: manager.appWidth
    state:"metadata"

    Component.onCompleted: {
        manager.setBoundStatus(true);
        nav_row.navSource="NavOptions5.qml";
        info_panel.state="hidden";
        controlComponent = "Controls_5.qml" //controlsLoader.sourceComponent
    }


    property string metadataComponent:"Metadata_"+manager.i_current_mediaType+".qml"
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property string controlComponent: ""
    property alias playlistSource:playlist.model
    property alias playlistDelegate:playlist.delegate
    property bool enableScrollbar:true
    property string label:"foo"

    Item{
        id: options_display
    }

    Item{
        id:now_playing_image_container
    }


      AdvancedMediaOptions{
          id:options
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

    states: [
        State {
            name: "metadata"
            AnchorChanges{
                target:metaDataPanel
                anchors.right: undefined
                anchors.left: media_playback_base.left
            }

            PropertyChanges {
                target: hiddenDrag
                x:media_playback_base.x / 2

            }
            PropertyChanges {
                target: options
                state:"unloaded"
            }
        },
        State {
            name: "playlist"
            PropertyChanges {
                target: options
                state:"unloaded"
            }
            AnchorChanges{
                target: metaDataPanel
                anchors.left: undefined
                anchors.right:media_playback_base.left
            }
        },
        State {
            name: "controls"

            PropertyChanges {
                target: options
                state:"loaded"
            }

            AnchorChanges{
                target: metaDataPanel
                anchors.right: undefined
                anchors.left: media_playback_base.right
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
