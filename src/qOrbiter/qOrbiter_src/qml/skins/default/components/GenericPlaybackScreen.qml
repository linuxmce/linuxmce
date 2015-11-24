import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import enums.media 1.0
import QtQuick.Controls 1.2
import "../."
Panel{
    id:generic_playback_panel
    headerTitle:MediaHelper.translateType(manager.i_current_mediaType)
    content: Item {
        id:generic_playback
        property bool showingPlaylist:false
        anchors.fill: parent
        Component.onCompleted: manager.setBoundStatus(true)
        NowPlayingImage {
            id: imgContainer
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

        Item{
            id:lightingAudioControls
            anchors.right: parent.right
            height:(manager.isProfile ? Style.scaleY(8) : Style.scaleY(10) ) *3
            width:(manager.isProfile ? Style.scaleX(14) : Style.scaleX(10) ) *2
            Item{
                id:lightingControls
                width:(parent.width/2)-5
                height: parent.height
                anchors.right: parent.right

                Column{
                    anchors.fill: parent
                    StyledButton{
                        state:"round"
                        height: Style.scaleY(8)
                        buttonText: "+"
                        onActivated: manager.adjustLights("+10")
                    }
                    StyledButton{
                        state:"round"
                        height: Style.scaleY(8)
                        buttonText: qsTr("Floorplan")
                        onActivated: manager.currentScreen="Screen_2.qml"
                    }
                    StyledButton{
                        state:"round"
                        height: Style.scaleY(8)
                        buttonText: "-"
                        onActivated: manager.adjustLights("-10")

                    }
                }

            }

            Item{
                id:audioControls
                width:parent.width/2
                height: parent.height
                anchors.left: parent.left

                Column {
                    id:leftCol
                    height: childrenRect.height
                    width: childrenRect.width
                    anchors{
                        left:parent.left
                        top:parent.top.top
                    }

                    StyledButton {
                        state:"round"
                        id: btVolUp
                        height: Style.scaleY(8)
                        buttonText: qsTr("Vol. +", "Media Volume")
                        onActivated: manager.adjustVolume(+1)
                    }

                    StyledButton {
                        id: mute
                        state:"round"
                        buttonText: ""
                        height: Style.scaleY(8)
                        onActivated: manager.mute()
                        Image{
                            anchors.centerIn: parent
                            source: "../images/mute_up.png"
                            anchors.fill: parent
                        }
                    }
                    StyledButton {
                        id: btVolDown
                        state:"round"
                        buttonText: qsTr("Vol. -", "Media Volume")
                        height: Style.scaleY(8)
                        onActivated: manager.adjustVolume(-1)
                    }
                }

            }
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
                when:manager.i_current_mediaType == MediaTypes.LMCE_StoredAudio
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
            },
            State{
                when:manager.i_current_mediaType== MediaTypes.LMCE_StreamedAudio || manager.i_current_mediaType == MediaTypes.NP_InternetMedia
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
                    target: imgContainer
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
                when:manager.i_current_mediaType == MediaTypes.NP_NetworkMedia
                name:"networkmedia"

                PropertyChanges{
                    target: playlistPanel
                    visible:false
                }

                PropertyChanges {
                    target: imgContainer
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
                when: manager.i_current_mediaType==66
                extend:"networkmedia"

            },
            State{
                name:"options"
            }
        ]

    }
}


