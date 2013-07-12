import QtQuick 2.0
Item{
    id:media_playback_base
    height: manager.appHeight
    width: manager.appWidth
    state:"metadata"

    Component.onCompleted: manager.setBoundStatus(true)
    property string metadataComponent:"Metadata_"+manager.i_current_mediaType+".qml"
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property string controlComponent: "Controls_"+manager.i_current_mediaType+".qml" //controlsLoader.sourceComponent
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
        id:controlPanel
        height: parent.height
        width: parent.width
        clip:true
        anchors.right: metaDataPanel.left

        StyledText{
            id:ct
            text:"Controls go here!"
            anchors.centerIn: parent
            fontSize: 48
            color:"white"
        }

        MouseArea{
            anchors.fill: parent
            onReleased: media_playback_base.state="metadata"
        }
    }

    Item{
        id:metaDataPanel
        height: parent.height
        width: parent.width
        clip:true
        MouseArea{
            id:dragHandler
            anchors.fill: parent
            drag.target: hiddenDrag
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: parent.width

            onReleased:  {

                console.log(hiddenDrag.x)

                if(hiddenDrag.x  < parent.width /2 ){
                    console.log("playlist") ;
                   media_playback_base.state = "playlist"
                }
                else {
                    media_playback_base.state = "controls";
                    console.log("controls")
                }
            }

           // onReleased: media_playback_base.state="playlist"
        }

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
        },
        State {
            name: "playlist"
            AnchorChanges{
                target: metaDataPanel
                anchors.left: undefined
                anchors.right:media_playback_base.left
           }
        },
        State {
            name: "controls"
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
                duration: 500
                easing.type: Easing.SineCurve
                easing.amplitude: 1
            }
        }
    ]

}
