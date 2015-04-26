import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import "../."
Item {
    id:generic_playback
    property bool showingPlaylist:false

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

    Item{
        id:lightingAudioControls
        anchors.right: parent.right
        width: Style.scaleX(12)
        height: parent.height*.15

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

    NowPlayingImage {
        id: imgContainer
        anchors{
            verticalCenter: parent.verticalCenter
            left:playlistPanel.right
        }
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
        }
    ]

}
