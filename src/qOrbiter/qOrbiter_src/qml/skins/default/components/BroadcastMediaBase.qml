import QtQuick 2.2

import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import "../."
Item{
    id:media_playback_base
    anchors.fill: parent


    function resetState(){
        switch(manager.i_current_mediaType){

        case MediaTypes.LMCE_LiveTV:
        case MediaTypes.NP_LiveTV:
            media_playback_base.state="tuner"
            break;

        case MediaTypes.LMCE_StreamedAudio:
            media_playback_base.state="streamingaudio"
            break;

        case MediaTypes.LMCE_LiveRadio:
            media_playback_base.state="radio"
            break;

        case MediaTypes.LMCE_AirPlay_photos:
            media_playback_base.state="airplay"
            break;
        case MediaTypes.NP_NetworkMedia:
            media_playback_base.state="networkmedia"
            break;

        default:
            media_playback_base.state="fallback"
            break;
        }
    }

    Row{
        id:controlRow

        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
            margins: 5
        }
        height: Style.appNavigation_panelHeight
        spacing: 5

        StyledButton{
            id:home
            state: "remote"
            buttonText: qsTr("Home", "Return to screen 1")
            anchors.verticalCenter: parent.verticalCenter
            onActivated: {
                manager.setCurrentScreen(Screens.Main)
            }
        }

        StyledButton{
            id:options
            state: "remote"
            anchors.verticalCenter: parent.verticalCenter
            buttonText: media_playback_base.state==="options" ? "Remote" : qsTr("Options")
            onActivated: {
                if(media_playback_base.state==="options"){
                    resetState()
                } else {
                    media_playback_base.state="options"
                }
            }
        }

        StyledButton{
            id:plst
            state: "remote"
            anchors.verticalCenter: parent.verticalCenter
            buttonText: qsTr("Channels", "Tv Channel listing")
            onActivated: {
                if(directionDiamond.visible)
                    showingPlaylist=!showingPlaylist
            }
        }
        StyledButton{
            id:btns
            state: "remote"
            anchors.verticalCenter: parent.verticalCenter
            buttonText: qsTr("Buttons", "Additional Tv Buttons")
            onActivated: {
                if(media_playback_base.state=="buttongrid"){
                    resetState()
                }else {
                    media_playback_base.state="buttongrid"
                }
            }
        }
    }

    property string screenLabel:"foo"
    property bool usePlaylist:true
    property string metadataComponent:"Metadata_"+manager.i_current_mediaType+".qml"

    property string controlComponent: ""
    property bool enableScrollbar:true
    property bool showingPlaylist:true;

    onShowingPlaylistChanged: {
        console.log(showingPlaylist)    }

    //    Connections{
    //        target:qml_root
    //        onShowMetadata:{
    //            state="metatdata"
    //        }
    //        onShowPlaylist:{
    //            state="playlist"
    //        }
    //        onShowAdvanced:{
    //            state="controls"
    //        }
    //    }

    Component.onCompleted: {
        manager.setBoundStatus(true)
        forceActiveFocus()
        resetState()
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


    Item{
        id: options_display
    }

    Item{
        id:now_playing_image_container
    }

    AdvancedMediaOptions {
        id: controlPanel
    }

    NowPlayingImage{
        id:npImage
        anchors{
            left:playlistPanel.right
            top:parent.bottom
            leftMargin: 5
        }
    }

    Image {
        id: dvdImage
        width: Style.scaleX(65)
        height:Style.scaleY(65)
        fillMode: Image.PreserveAspectFit
        source: ""
        visible:false
        anchors{
            top:parent.top
            left: parent.left
            margins: Style.scaleX(2)
        }
    }


    ControlDiamond{
        id:arrows
        anchors{
            verticalCenter:numbers.verticalCenter
            horizontalCenter: numbers.horizontalCenter
        }

        visible:!numbers.visible
    }

    DirectionPad {
        id: directionDiamond
        anchors.left: manager.isProfile ? playlistPanel.right : numbers.right
        anchors.bottom: parent.bottom
    }

    MediaNumberPad{
        id:numbers
        anchors.left: playlistPanel.right
        anchors.top: playlistPanel.top
        visible:!media_playback_base.showingPlaylist
    }

    TvButtonPanel{
        id:buttonPanel
        anchors{
            top:controlRow.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        visible:false
    }

    LightingAudioControls {
        id: lightingAudioControls
        anchors{
            top:parent.bottom
            right:parent.right
        }
    }

    Loader{
        id:mediaTypeMetaData
        width: parent.width *.35
        //source: metadataComponent
        anchors{
            left: npImage.left
            top:npImage.bottom
            bottom: mediaScrollerTarget.top
        }
    }

    MediaTransport{
        id:mediaScrollerTarget
        state:dceTimecode.tcTotalTime!==0 ? "up" : "down"
    }

    TvChannelPanel{
        id:playlistPanel
        clip: true
        anchors{
            top:controlRow.bottom
            left:parent.left
            bottom:parent.bottom
        }
        width: Style.listViewWidth_medium
    }

    states: [
        State {
            name: "fallback"
            extend: ""
        },
        State {
            name: "tuner"
            extend:""
            PropertyChanges {
                target: media_playback_base
                showingPlaylist: playlistPanel.itemCount > 1 ? true : false
            }
            PropertyChanges {
                target: directionDiamond
                visible:true
            }

        },
        State {
            extend:"tuner"
            name: "cable"

        },
        State{
            name:"networkmedia"
            PropertyChanges {
                target: media_playback_base
                showingPlaylist: false
            }
            PropertyChanges {
                target: directionDiamond
                visible:true
            }
            PropertyChanges {
                target: options
                visible:false
            }
            PropertyChanges {
                target: plst
                visible:false
            }
            PropertyChanges {
                target: mediaScrollerTarget
                visible:false

            }
        },
        State{
            name:"streamingaudio"
        },
        State{
            name:"radio"
            PropertyChanges {
                target: mediaScrollerTarget
                visible:false

            }
        },
        State{
            name:"options"
            PropertyChanges{
                target: npImage
                visible:false
            }
            PropertyChanges{
                target: numbers
                visible:false
            }
            PropertyChanges{
                target: dvdImage
                visible:false
            }
            PropertyChanges{
                target:arrows
                visible:false
            }
            PropertyChanges {
                target: directionDiamond
                visible:false
            }
            PropertyChanges{
                target: lightingAudioControls
                visible:false
            }
            PropertyChanges {
                target: mediaTypeMetaData
                visible:false
            }
            PropertyChanges {
                target: mediaScrollerTarget
                visible:false
            }
            PropertyChanges {
                target: playlistPanel
                visible:false
            }
            PropertyChanges {
                target: controlPanel
                state:"loaded"
            }
        },
        State {
            name: "buttongrid"
            extend:"options"
            PropertyChanges {
                target: controlPanel
                state:"unloaded"
            }
            PropertyChanges {
                target: buttonPanel
                visible:true
            }
        }
    ]

    //    transitions: [
    //        Transition {
    //            from: "*"
    //            to: "*"
    //            AnchorAnimation{
    //                duration:Style.appButtonLargeHeightnimation_medium
    //                easing.type: skinStyle.animation_easing
    //            }
    //        },
    //        Transition {
    //            from: "*"
    //            to: "options"
    //            PropertyAnimation{
    //                duration:skinStyle.animation_medium
    //                easing.type: skinStyle.animation_easing
    //            }

    //        }
    //    ]
}


