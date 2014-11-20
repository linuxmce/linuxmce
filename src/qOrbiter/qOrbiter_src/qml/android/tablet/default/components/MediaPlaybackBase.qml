import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
import org.linuxmce.enums 1.0

Panel{
    id:media_playback_base
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
                manager.setCurrentScreen("Screen_1.qml")
            }
        }

        StyledButton{
            id:options
            anchors.verticalCenter: parent.verticalCenter
            buttonText: qsTr("Media Options")
            onActivated: {
                manager.setCurrentScreen("Screen_1.qml")
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
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
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
        controlComponent = "Controls_5.qml"
        // nav_row.navSource="NavOptions5.qml";

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
        anchors.bottom: mediaScrollerTarget.top
        visible:true
    }


    DirectionDiamond {
        id: directionDiamond
        visible:false
    }

    Loader{
        id:mediaTypeMetaData
        width: parent.width *.40
        source: metadataComponent
        anchors{
            left: npImage.left
            top:npImage.bottom
            bottom: mediaScrollerTarget.top
        }
    }

    Loader{
        id:mediaScrollerTarget
        sourceComponent: MediaScrollBar{}
        anchors{
            bottom:parent.bottom
            left:parent.left
            right:parent.right
            margins: 10
        }
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
        width: parent.width *.35
        clip: true
    }

    states: [
        State {
            name: "fallback"
            extend: ""
        },
        State {
            name: "localvideo"

        },
        State {
            name: "localaudio"

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
            name:"playlistopen"
            when:showingPlaylist
            AnchorChanges{
                target: playlistPanel
                anchors{
                    right:undefined
                    left:media_playback_base.left
                }
            }
        },
        State{
            name:"playlistclosed"
            when:!showingPlaylist
            AnchorChanges{
                target: playlistPanel
                anchors{
                    left:undefined
                    right:media_playback_base.left
                }
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
        }
    ]

}
