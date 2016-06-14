import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import enums.media 1.0
import QtQuick.Controls 1.1
import "../."
Panel{
    id:generic_playback_panel
    headerTitle:MediaHelper.translateType(manager.i_current_mediaType)

    Component{
        id:transferMediaPrompt
        NowPlayingTransferMedia {
            id: transferMedia
        }
    }

    Component{
        id:standardButtons
        Row{
            width: children.length*Style.appButtonWidth
            height:parent.height
            StyledButton{
                id:resend
                height:Style.appNavigation_panelHeight

                buttonText: qsTr("Resend AV", "Resend AV Commands")
                onActivated: manager.currentScreen="Screen_38.qml"
            }

            StyledButton{
                id:power_btn
                height:Style.appNavigation_panelHeight
                buttonText: qsTr("Power Off", "Turn off Device")
                onConfirm:{
                    manager.exitMediaMenu()
                    manager.stopMedia()
                }
            }

            StyledButton{
                id:options
                height:Style.appNavigation_panelHeight
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

                height:Style.appNavigation_panelHeight
                buttonText: qsTr("Buttons", "Additional Media Buttons")
                onActivated: {
                    if(contentItem.state=="buttongrid"){
                        contentItem.resetState()
                    }else {
                        contentItem.state="buttongrid"
                    }
                }
            }
            StyledButton{
                id:transfer
                height:Style.appNavigation_panelHeight
                buttonText: qsTr("Transfer", "Move media")
                onActivated: qmlRoot.createPopup(transferMediaPrompt)
            }
        }
    }

    Component{
        id:dvdMenuButtons
        Row{
            height: parent.height
            width: 400
            StyledButton{
                id:resend
                height:Style.appNavigation_panelHeight
                buttonText: qsTr("Resend AV", "Resend AV Commands")
                onActivated: manager.currentScreen="Screen_38.qml"
            }

            StyledButton{
                id:options
                height:Style.appNavigation_panelHeight
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
                height:Style.appNavigation_panelHeight
                buttonText: qsTr("Buttons", "Additional Media Buttons")
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



    //    buttonContent:[

    //        StyledButton{
    //            id:resend
    //            height:Style.appNavigation_panelHeight

    //            buttonText: qsTr("Resend AV", "Resend AV Commands")
    //            onActivated: manager.currentScreen="Screen_38.qml"
    //        },

    //        StyledButton{
    //            id:power_btn
    //            height:Style.appNavigation_panelHeight
    //            buttonText: qsTr("Power Off", "Turn off Device")
    //            onConfirm:{
    //                manager.exitMediaMenu()
    //                manager.stopMedia()
    //            }
    //        },

    //        StyledButton{
    //            id:options
    //            height:Style.appNavigation_panelHeight
    //            buttonText: contentItem.state==="options" ? "Remote" : qsTr("Options")
    //            onActivated: {
    //                if(contentItem.state==="options"){
    //                    contentItem.resetState()
    //                } else {
    //                    contentItem.state="options"
    //                }
    //            }
    //        },

    //        StyledButton{
    //            id:btns

    //            height:Style.appNavigation_panelHeight
    //            buttonText: qsTr("Buttons", "Additional Media Buttons")
    //            onActivated: {
    //                if(contentItem.state=="buttongrid"){
    //                    contentItem.resetState()
    //                }else {
    //                    contentItem.state="buttongrid"
    //                }
    //            }
    //        }
    //    ]

    content: Item {
        id:contentItem
        property bool showingPlaylist:false
        anchors.fill: parent
        Component.onCompleted: {manager.setBoundStatus(true); resetState()}

        function resetState(){

            if(manager.currentScreen=="Screen_50.qml"){
                generic_playback_panel.buttonControls = dvdMenuButtons
            } else {
                generic_playback_panel.buttonControls = standardButtons
            }

            switch(manager.i_current_mediaType){

            case MediaTypes.LMCE_StoredAudio:
                state="localaudio"
                break;

            case MediaTypes.LMCE_StreamedAudio:
            case MediaTypes.NP_InternetMedia:
                state="streamingaudio"
                break;

            case MediaTypes.LMCE_LiveRadio:
                state="radio"
                break;

            case MediaTypes.NP_NetworkMedia:
                state="networkmedia"
                break;
            case MediaTypes.NP_NetworkMedia:
            case 66:
                state="networkmedia"
                break;

            case MediaTypes.LMCE_DVD:
                state= manager.currentScreen=="Screen_50.qml" ? "dvdmenu" : "dvd"
                break;


            default:
                state="fallback"
                break;
            }
        }

        NowPlayingImage {
            id: npImage

            Connections{
                target:manager
                onIsProfileChanged:{
                    if(manager.isProfile){
                        anchors.verticalCenter = undefined
                        anchors.top = generic_playback.top
                    } else {
                        anchors.top = undefined
                        anchors.verticalCenter = generic_playback.verticalCenter
                    }
                }
            }

            anchors{
                left:playlistPanel.right
            }
        }

        Image{
            id:dvdImage
            Connections{
                target:manager
                onIsProfileChanged:{
                    if(manager.isProfile){
                        anchors.verticalCenter = undefined
                        anchors.top = generic_playback.top
                    } else {
                        anchors.top = undefined
                        anchors.verticalCenter = generic_playback.verticalCenter
                    }
                }
            }

            anchors{
                left:playlistPanel.right
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
                running: contentItem.state==="dvdmenu"
                onTriggered: manager.grabStreamImage()
            }

        }

        Item{
            id:directionDiamond
        }

        DirectionPad {
            id: arrows
            anchors{
                right:parent.right
                bottom:mediaScrollerTarget.bottom
            }
        }

        ControlDiamond{
            id:control_diamond
            visible:false
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }

        LightingAudioControls{
            id: lightingAudioControls
            anchors{
                top:parent.bottom
                right:parent.right
            }
        }

        TvButtonPanel{
            id:buttonPanel
            anchors.fill: parent
            visible:false
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

        AdvancedMediaOptions {
            id: controlPanel
        }

        MediaTransport {
            id: mediaScrollerTarget
            height: parent.height *.04
            width: parent.width *.75
            anchors{
                bottom:transport_buttons.top
                horizontalCenter: parent.horizontalCenter
                bottomMargin:10
            }
        }

        Slider{
            id:altTransport
            height: parent.height *.04
            width: parent.width *.75
            visible:mediaScrollerTarget.visible
            updateValueWhileDragging: false
            tickmarksEnabled: true

            minimumValue: 0
            maximumValue: roomList.currentEaTimecode.timecodeLength
            value:roomList.currentEaTimecode.timecodePosition
            onPressedChanged: {
                if(!pressed)
                    roomList.currentEaTimecode.finishDragging(value);
            }

            anchors{
                bottom:transport_buttons.top
                horizontalCenter: parent.horizontalCenter
                bottomMargin:10
            }
        }

        PlaylistPanel {
            id: playlistPanel
        }

        TransportButtons {
            id: transport_buttons
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
                    target: generic_playback
                    showingPlaylist: playlistPanel.itemCount > 1 ? true : false

                }
            },
            State {
                name: "localaudio"

                PropertyChanges {
                    target: arrows
                    visible:false
                }
                PropertyChanges{
                    target:directionDiamond
                    visible:false
                }
                PropertyChanges{
                    target: control_diamond
                    visible:false
                }

            },
            State{

                name:"dvd"

                PropertyChanges {
                    target: transport_buttons
                    visible:false
                }
                PropertyChanges {
                    target: control_diamond
                    visible:true
                }
                PropertyChanges {
                    target:arrows
                    visible:false
                }

            },
            State{
                name:"dvdmenu"
                PropertyChanges {
                    target: transport_buttons
                    visible:false
                }
                PropertyChanges {
                    target: control_diamond
                    visible:false
                }
                PropertyChanges {
                    target:arrows
                    visible:true
                }
                PropertyChanges{
                    target: playlistPanel
                    visible:false
                }

                PropertyChanges {
                    target: npImage
                    visible:false
                }
                PropertyChanges {
                    target: dvdImage
                    visible:true
                }

            },

            State{

                name:"streamingaudio"
                extend: "networkmedia"

                PropertyChanges {
                    target: transport_buttons
                    visible:false
                }
                PropertyChanges {
                    target: control_diamond
                    visible:true
                }
                PropertyChanges {
                    target:arrows
                    visible:false
                }
                PropertyChanges {
                    target: npImage
                    visible:true
                }

            },
            State{
                name:"radio"
            },
            State{
                name:"airplay"
            },
            State{

                name:"networkmedia"

                PropertyChanges{
                    target: playlistPanel
                    visible:false
                }

                PropertyChanges {
                    target: npImage
                    visible:false
                }

                PropertyChanges {
                    target: arrows
                    visible:true
                }

                PropertyChanges{
                    target: control_diamond
                    visible:false
                }
                PropertyChanges{
                    target: directionDiamond
                    visible:false
                }

                PropertyChanges{
                    target: mediaScrollerTarget
                    visible:false
                }
                PropertyChanges {
                    target: transport_buttons
                    visible:false
                }

            },
            State {
                name: "internetmedia"

                extend:"networkmedia"

            },
            State{
                name:"options"
                PropertyChanges{
                    target: npImage
                    visible:false
                }
                //                PropertyChanges{
                //                    target: numbers
                //                    visible:false
                //                }
                PropertyChanges{
                    target: mediaScrollerTarget
                    visible:false
                }
                PropertyChanges{
                    target: dvdImage
                    visible:false
                }
                PropertyChanges {
                    target: transport_buttons
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

    }
}


