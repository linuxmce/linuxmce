import QtQuick 2.2

import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0

Panel{
    id:media_playback_base
    headerTitle: manager.translateMediaType(manager.i_current_mediaType)
    fillColor: "grey"
    headerFillColor: "black"
    headerRect.opacity: .65

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
            right:headerRect.right
            top:headerRect.top
            bottom:headerRect.bottom
            margins: 5
        }
        width: headerRect.width/2
        spacing: 5

        StyledButton{
            id:home
            buttonText: qsTr("Home", "Return to screen 1")
            anchors.verticalCenter: parent.verticalCenter
            onActivated: {
                manager.setCurrentScreen(Screens.Main)
            }
        }

        StyledButton{
            id:options
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
            anchors.verticalCenter: parent.verticalCenter
            buttonText: qsTr("Channels", "Tv Channel listing")
            onActivated: {
                if(directionDiamond.visible)
                    showingPlaylist=!showingPlaylist
            }
        }
        StyledButton{
            id:btns
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


    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
        bottom:parent.bottom
    }


    property string screenLabel:"foo"
    property bool usePlaylist:true
    property string metadataComponent:"Metadata_"+manager.i_current_mediaType+".qml"
    property Item scrollBarComponent:undefined
    property string controlComponent: ""
    property Item playlistSource
    property Item playlistDelegate
    property bool enableScrollbar:true
    property Component playListComponent:PlaylistView{}
    property bool showingPlaylist:true;

    onShowingPlaylistChanged: {
        console.log(showingPlaylist)    }

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
        manager.setBoundStatus(true)
        forceActiveFocus()
        info_panel.state="hidden";
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
            top:headerRect.bottom
            leftMargin: 5
        }
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


    ControlDiamond{
        id:arrows
        anchors.bottom: mediaScrollerTarget.top
        visible:true
    }

    MediaNumberPad{
        id:numbers
        anchors.centerIn: parent
        visible:!showingPlaylist

    }

    DirectionDiamond {
        id: directionDiamond
        visible:false
        anchors.verticalCenter: undefined
        anchors.left: playlistPanel.right
        anchors.bottom: parent.bottom
    }
    TvButtonPanel{
        id:buttonPanel
        anchors{
            top:headerRect.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }

        visible:false
    }

    LightingAudioControls {
        id: lightingAudioControls
        anchors{
            top:headerRect.bottom
            right:parent.right
        }
    }

    Loader{
        id:mediaTypeMetaData
        width: parent.width *.35
        source: metadataComponent
        anchors{
            left: npImage.left
            top:npImage.bottom
            bottom: mediaScrollerTarget.top
        }
    }

    MediaScrollBar{
        id:mediaScrollerTarget
        state:dceTimecode.tcTotalTime!==0 ? "up" : "down"
    }

    TvChannelPanel{
        id:playlistPanel
        anchors{
            top:media_playback_base.headerRect.bottom
            bottom:mediaScrollerTarget.top
            left:  parent.left
            right:  undefined
            leftMargin:  5
        }
        width: parent.width *.30
        clip: true
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
            PropertyChanges {
                target: mediaScrollerTarget
                visible:false

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

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration:skinStyle.animation_medium
                easing.type: skinStyle.animation_easing
            }
        },
        Transition {
            from: "*"
            to: "options"
            PropertyAnimation{
                duration:skinStyle.animation_medium
                easing.type: skinStyle.animation_easing
            }

        }
    ]

}
