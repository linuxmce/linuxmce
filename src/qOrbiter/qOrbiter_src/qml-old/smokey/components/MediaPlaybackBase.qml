import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
Item{
    id:media_playback_base
   anchors{
    top:parent.top
    left:parent.left
    right:parent.right
    bottom:parent.bottom
   }
    state:"metadata"

    property string screenLabel:"foo"

    Connections{
        target:qml_root
        onShowMetadata:{
            state="metatdata"
        }
        onShowPlaylist:{
            state="playlist"
        }
        onShowAdvanced:{
            state="controls"
        }
    }

    Component.onCompleted: {
        forceActiveFocus()
        if(media_playback_base.state!=="dvdmenu"){
            nav_row.navSource="NavOptions5.qml";
            info_panel.state="hidden";
            controlComponent = "Controls_5.qml" //controlsLoader.sourceComponent
        } else {

        }
        manager.getStoredPlaylist()
        controlComponent = "Controls_5.qml"
        nav_row.navSource="NavOptions5.qml";
        info_panel.state="hidden";
    }



    Connections{
        target: manager
        onMediaScreenShotReady: dvdImage.source="image://listprovider/screenshot/"+securityvideo.timestamp
    }

    Timer{
        id:streamtimer
        repeat: true
        interval: 1000
        triggeredOnStart: true
        running: media_playback_base.state==="dvdmenu"
        onTriggered: manager.grabStreamImage()
    }

    property string metadataComponent:"Metadata_"+manager.i_current_mediaType+".qml"
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property string controlComponent: ""
    property Item playlistSource
    property Item playlistDelegate
    property bool enableScrollbar:true
    property Component playListComponent:PlaylistView{}

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
        Image {
            id: dvdImage
            width: scaleX(65)
            height:scaleY(65)
            fillMode: Image.PreserveAspectFit
            source: ""
            visible:false
            anchors{
                top:parent.top
                left: parent.left
                margins: scaleX(2)
            }
        }

        Item{
            id:directionDiamond
            height: dvdImage.height/2
            width: height
            visible:dvdImage.visible
            anchors{
                right: parent.right
                verticalCenter: dvdImage.verticalCenter
                margins: scaleX(5)
            }

            Rectangle{
                id:square
                anchors.fill: parent
                color: "black"
                opacity: .50
                rotation: 0
                visible:true
            }

//            Rectangle{
//                id:circle
//                height: parent.height
//                width: parent.width
//                radius: height
//                color: "black"
//                opacity: .75
//                border.color: "white"
//                border.width: 2
//                anchors.centerIn: square
//            }

            StyledButton{
                id:up
                anchors{
                    horizontalCenter: square.horizontalCenter
                    verticalCenter: square.top
                }
                buttonText: "Up"
                ArrowUpHandler{
                }
            }

            StyledButton{
                id:dn
                anchors{
                    horizontalCenter: square.horizontalCenter
                    verticalCenter: square.bottom
                }
                buttonText: "Down"
                ArrowDownHandler{
                }
            }

            StyledButton{
                id:left
                anchors{
                    horizontalCenter: square.left
                    verticalCenter: square.verticalCenter
                }
                buttonText: "Left"
                ArrowLeftHandler{
                }
            }
            StyledButton{
                id:right
                anchors{
                    horizontalCenter: square.right
                    verticalCenter: square.verticalCenter
                }
                buttonText: "Right"
                ArrowRightHandler{
                }
            }
            StyledButton{
                id:ok
               anchors.centerIn: square
                buttonText: "Enter"
                EnterButtonHandler{
                }
            }


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
            sourceComponent: playListComponent
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

        }, State {
            name: "dvdmenu"

            PropertyChanges {
                target: npImage
                visible:false
            }
            PropertyChanges {
                target: dvdImage
                visible:true
            }
            PropertyChanges {
                target: mediaTypeMetaData
                source:""
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
