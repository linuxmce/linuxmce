import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import "../."
Item {
    id:generic_playback
    property bool showingPlaylist:false
    NowPlayingImage {
        id: imgContainer
        anchors{
            verticalCenter: parent.verticalCenter
            left:!manager.isProfile ? playlistPanel.right :generic_playback.left
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
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    Item{
        id:lightingAudioControls
        anchors.right: parent.right
        height:(manager.isProfile ? Style.scaleY(8) : Style.scaleY(10) ) *3
        width:(manager.isProfile ? Style.scaleX(14) : Style.scaleX(10) ) *2
        Item{
            id:lightingControls
            width:parent.width/2
            height: parent.height
            Rectangle{
                anchors.fill: parent
                color:"black"
            }
            Column{
                anchors.fill: parent
                StyledButton{
                    state:"remote"
                    buttonText: "+"
                    onActivated: manager.adjustLights("+10")
                }
                StyledButton{
                    state:"remote"
                    buttonText: "floorplan"
                }
                StyledButton{
                    state:"remote"
                    buttonText: "-"
                    onActivated: manager.adjustLights("-10")

                }
            }

        }

        Item{
            id:audioControls
            width:parent.width/2
            height: parent.height
            anchors.right: parent.right
            Rectangle{
                anchors.fill: parent
                color:"black"
            }
            Column{
                anchors.fill: parent
                StyledButton{
                    state:"remote"
                    buttonText: "+"
                    onActivated: manager.adjustVolume(+1)
                }
                StyledButton{
                    state:"remote"
                    buttonText: "Mute"
                    onActivated: manager.mute()
                }
                StyledButton{
                    state:"remote"
                    buttonText: "-"
                    onActivated:manager.adjustVolume(-2)
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
                visible:true
            }


            PropertyChanges{
                target: mediaScrollerTarget
                visible:false
            }

            AnchorChanges{
                target: transport_buttons
                anchors.bottom: parent.bottom
            }

        },
        State{
            name:"options"
        }
    ]

}
