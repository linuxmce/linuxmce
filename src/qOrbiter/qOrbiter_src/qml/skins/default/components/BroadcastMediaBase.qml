import QtQuick 2.2

import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import "../."
Panel{
    id:media_playback_base_panel
    headerTitle: manager.translateMediaType(manager.i_current_mediaType)

        Component{
            id:broadcastBtns
            Row{
                width: children.length*appStyle.appButtonWidth
                height:parent.height
                StyledButton{
                    id:resend
                     height:appStyle.appNavigation_panelHeight
                    buttonText: qsTr("Resend AV", "Resend AV Commands")
                    onActivated: manager.currentScreen="Screen_38.qml"
                }

                StyledButton{
                    id:power_btn
                    height:appStyle.appNavigation_panelHeight
                    buttonText: qsTr("Power Off", "Turn off Device")
                    onConfirm:{
                        manager.exitMediaMenu()
                        manager.stopMedia()
                    }
                }

                StyledButton{
                    id:options
                    height:appStyle.appNavigation_panelHeight
                    buttonText: contentItem.state==="options" ? "Remote" : qsTr("Options")
                    onActivated: {
                        if(contentItem.state==="options"){
                            contentItem.resetState()
                        } else {
                            contentItem.state="options"
                        }
                    }
                }

                StyledButton{
                    id:btns

                    height:appStyle.appNavigation_panelHeight
                    buttonText: qsTr("Buttons", "Additional Tv Buttons")
                    onActivated: {
                        if(contentItem.state=="buttongrid"){
                            contentItem.resetState()
                        }else {
                            contentItem.state="buttongrid"
                        }
                    }
                }
            }
        }


    content: Item{
        anchors.fill: parent
        id:media_playback_base
        property string screenLabel:"foo"
        property bool usePlaylist:true
        property string metadataComponent:"Metadata_"+manager.i_current_mediaType+".qml"
        property string controlComponent: ""
        property bool enableScrollbar:true
        property bool showingPlaylist:true;
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
            case 66:
                media_playback_base.state="networkmedia"
                break;

            default:
                media_playback_base.state="fallback"
                break;
            }
        }

        Component.onCompleted: {
            manager.setBoundStatus(true)
            forceActiveFocus()
            resetState()
            buttonControls = broadcastBtns
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
            width: appStyle.scaleX(65)
            height:appStyle.scaleY(65)
            fillMode: Image.PreserveAspectFit
            source: ""
            visible:false
            anchors{
                top:parent.top
                left: parent.left
                margins: appStyle.scaleX(2)
            }
        }


        ControlDiamond{
            id:arrows
            height:manager.isProfile ? appStyle.scaleY(35) :appStyle.scaleY(50)
            anchors{
                verticalCenter:numbers.verticalCenter
                horizontalCenter: numbers.horizontalCenter
            }

            visible:!numbers.visible
        }

        DirectionPad {
            id: directionDiamond
            anchors.left: manager.isProfile ? undefined: numbers.right
            anchors.horizontalCenter: manager.isProfile ? numbers.horizontalCenter : undefined
            anchors.verticalCenter: manager.isProfile ? undefined: parent.verticalCenter
            anchors.bottom: manager.isProfile ? parent.bottom : undefined

        }

        MediaNumberPad{
            id:numbers
            anchors.left: playlistPanel.right
            anchors.top: playlistPanel.top
            visible:true
        }

        TvButtonPanel{
            id:buttonPanel
            anchors.fill: parent
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
                top:parent.top
                left:parent.left
                bottom:parent.bottom
            }
            width: appStyle.listViewWidth_medium
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
                when:manager.i_current_mediaType===66

                PropertyChanges {
                    target: directionDiamond
                    visible:true
                }
                PropertyChanges {
                    target: options
                    visible:true
                }
                PropertyChanges {
                    target: playlistPanel
                    visible:false

                }
                PropertyChanges {
                    target: buttonPanel
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
        //                duration:appStyle.appButtonLargeHeightnimation_medium
        //                easing.type: skinappStyle.animation_easing
        //            }
        //        },
        //        Transition {
        //            from: "*"
        //            to: "options"
        //            PropertyAnimation{
        //                duration:skinappStyle.animation_medium
        //                easing.type: skinappStyle.animation_easing
        //            }

        //        }
        //    ]
    }

}


