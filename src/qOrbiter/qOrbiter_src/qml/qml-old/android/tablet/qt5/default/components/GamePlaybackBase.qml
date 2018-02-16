import QtQuick 2.2

import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0

Panel{
    id:game_playback_base
    headerTitle: manager.translateMediaType(manager.i_current_mediaType)
    fillColor: "grey"
    headerFillColor: "black"
    headerRect.opacity: .65
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
            buttonText: qsTr("Home")
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
                    switch(manager.i_current_mediaType){
                    case MediaTypes.LMCE_StoredAudio:
                        media_playback_base.state="localaudio"
                        break;

                    case MediaTypes.LMCE_StoredVideo:
                        media_playback_base.state="localvideo"
                        break;

                    case MediaTypes.LMCE_StreamedAudio:
                        media_playback_base.state="streamingaudio"
                        break;

                    case MediaTypes.LMCE_DVD:
                        media_playback_base.state="dvd"
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
                } else {
                    media_playback_base.state="options"
                }
            }
        }

        StyledButton{
            id:plst
            anchors.verticalCenter: parent.verticalCenter
            buttonText: qsTr("Playlist")
            onActivated: {
                showingPlaylist=!showingPlaylist
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

    property string controlComponent: ""
    property Item playlistSource
    property Item playlistDelegate
    property bool enableScrollbar:true
   // property Component playListComponent:playlistPanel
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
        info_panel.state="hidden";      ;

        switch(manager.i_current_mediaType){
        case MediaTypes.LMCE_Game_genesis:
            media_playback_base.state="localaudio"
            break;

        case MediaTypes.LMCE_StoredVideo:
            media_playback_base.state="localvideo"
            break;

        case MediaTypes.LMCE_StreamedAudio:
            media_playback_base.state="streamingaudio"
            break;

        case MediaTypes.LMCE_DVD:
            media_playback_base.state="dvd"
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


    DirectionDiamond {
        id: directionDiamond
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

    PlaylistPanel{
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
            name: "localvideo"
            extend:""
            PropertyChanges {
                target: media_playback_base
                showingPlaylist: playlistPanel.itemCount > 1 ? true : false

            }
        },
        State {
            name: "genesis"

        },
        State{
            name:"dvd"
        },
        State{
            name:"streamingaudio"
        },
        State{
            name:"radio"
        },
        State{
            name:"airplay"
        },
        State{
            name:"networkmedia"
        },
        State{
            name:"options"
            PropertyChanges{
                target: npImage
                opacity:0
            }
            PropertyChanges{
                target: dvdImage
                opacity:0
            }
            PropertyChanges{
                target:arrows
                opacity:0
            }
            PropertyChanges {
                target: directionDiamond
                opacity:0
            }
            PropertyChanges {
                target: lightingAudioControls
                opacity:0
            }
            PropertyChanges {
                target: mediaTypeMetaData
                opacity:0
            }
            PropertyChanges {
                target: mediaScrollerTarget
                opacity:0
            }
            PropertyChanges {
                target: playlistPanel
                opacity:0
            }
            PropertyChanges {
                target: controlPanel
                state:"loaded"

            }

        }


    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration:skinstyle.animation_medium
                easing.type: skinstyle.animation_easing
            }
        },
        Transition {
            from: "*"
            to: "options"
            PropertyAnimation{
                duration:skinstyle.animation_medium
                easing.type: skinstyle.animation_easing
            }

        }
    ]

}
