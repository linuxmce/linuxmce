import QtQuick 2.2
import "../."
import org.linuxmce.enums 1.0
Panel {
    useHeader: false
    content:
    Item {
        id: remote1
       anchors.fill: parent
        property int rowHeight:appStyle.scaleY(12)

        Column {
            id:leftCol
            height: childrenRect.height
            width: childrenRect.width
            anchors{
                left:parent.left
                top:mainDiamond.top
            }

            StyledButton {
                state:"round"
                id: btVolUp

                buttonText: qsTr("Vol. +", "Media Volume")
                onActivated: manager.adjustVolume(+1)
            }

            StyledButton {
                id: mute
                state:"round"
                buttonText: ""

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

                onActivated: manager.adjustVolume(-1)
            }
        }



        DirectionPad{
            id:mainDiamond
            visible: true
            height: appStyle.scaleY(12)*3
            width:  height
            anchors{
                top:parent.top
                horizontalCenter: parent.horizontalCenter
            }
        }

        Column { // Info, Up, Menu, Ch+
            id:rightCol
            spacing: manager.isProfile ? appStyle.scaleX(1) : appStyle.scaleY(1)
            height: childrenRect.height
            width: childrenRect.width
            anchors{
                right:parent.right
                top:parent.top
            }

            StyledButton {
                id: btChannelUp
                state:"round"
                buttonText: qsTr("Ch+", "Channel Up")
                onActivated: manager.tvChannelUp()
            }

            StyledButton {
                state:"round"
                id: btChannelDown
                buttonText: qsTr("Ch-", "Channel Down")
                onActivated: manager.tvChannelDown()
            }
        }



        Row { // Exit, Down, Guide, VolUp
            id:dvrRow
            width: childrenRect.width
            height: appStyle.appButtonHeight
            anchors{
                bottom:transports.top
                horizontalCenter: parent.horizontalCenter
            }
            StyledButton {
                state:"round"
                id: btExit
                buttonText: "Exit"
                onActivated: manager.exitMediaMenu()
            }
            StyledButton {
                id: btBack
                state:"round"
                buttonText: "Back"
                onActivated: manager.osdBack()
            }
            StyledButton {
                state:"round"
                id: btGuide
                buttonText: "Guide"
                onActivated: manager.extraButton("guide")
            }
            StyledButton {
                id: btInfo
                state:"round"
                buttonText: "Info"
                onActivated: manager.extraButton("info")
            }

            StyledButton {
                id: btMenu
                state:"round"
                buttonText: "Menu"
                onActivated: {
                    // AV devices seem to use 'showmenu' while VDR/Myth uses 'menu'
                    if (manager.i_current_mediaType == MediaTypes.LMCE_LiveTV) {
                        manager.extraButton("menu")
                    } else {
                        manager.extraButton("showmenu")
                    }
                }
            }
            StyledButton {
                id: btRecordings
                state:"round"
                buttonText: "Recordings"
                onActivated: manager.extraButton("recordings")
            }
        }



        Row {
            id:transports// SkipBack, Rewind, FastForward, SkipFwd

            width: childrenRect.width
            height: appStyle.appButtonHeight
            anchors{
                horizontalCenter: parent.horizontalCenter
                bottom:colorButtons.top
            }
            StyledButton {
                id: btStop
                state:"round"
                buttonText: ""
                height: manager.isProfile ? appStyle.scaleY(10) : appStyle.scaleY(12)
                // This is the Live TV remote, so we use the "Stop" CMD (AV control), and not the "Stop Media" CMD (DCE players)
                onActivated: manager.stop_AV()
                Image {
                    id: stopImg
                    source: "../images/stop_up.png"
                    height: parent.height/2
                    width: height
                    anchors.centerIn: parent
                }
            }
            StyledButton {
                id: btRewind
                state:"round"
                height: manager.isProfile ? appStyle.scaleY(10) : appStyle.scaleY(12)
                buttonText: ""
                Image {
                    id: rwImg
                    source: "../images/rw_up.png"
                    height: parent.height*.65
                    width: height
                    anchors.centerIn: parent
                }
                onActivated: manager.rewindMedia()
            }
            StyledButton {
                id: btPause
                state:"round"
                Image {
                    id: pauseImg
                    source: "../images/playpause_up.png"
                    height: parent.height*.65
                    width: height
                    anchors.centerIn: parent
                }
                height: manager.isProfile ? appStyle.scaleY(10) : appStyle.scaleY(12)
                buttonText: ""
                onActivated: manager.pauseMedia()
            }
            StyledButton {
                id: btPlay
                state:"round"
                height: manager.isProfile ? appStyle.scaleY(10) : appStyle.scaleY(12)
                buttonText: "Play"
                onActivated: manager.playResume()
            }
            StyledButton {
                id: btFFwd
                state:"round"
                height: manager.isProfile ? appStyle.scaleY(10) : appStyle.scaleY(12)
                buttonText: ""
                Image {
                    id: ffimg
                    source: "../images/ff_up.png"
                    height: parent.height*.65
                    width: height
                    anchors.centerIn: parent
                }
                onActivated: manager.fastForwardMedia();
            }

        }

        Row { // Red, Green, Yellow, Blue
            id:colorButtons
            spacing: manager.isProfile ? appStyle.scaleX(1) : appStyle.scaleY(1)
            height: appStyle.appButtonHeight
            width: childrenRect.width
            anchors{
                bottom:parent.bottom
                horizontalCenter: parent.horizontalCenter
                bottomMargin: 5
            }
            StyledButton {
                id: btRed
                state:"round"
                buttonText: qsTr("Red")
                phil: "red"

                onActivated: manager.redButtonPress()
            }
            StyledButton {
                id: btGreen
                state:"round"
                buttonText:qsTr( "Green")
                phil: "green"

                onActivated: manager.greenButtonPress()
            }
            StyledButton {
                id: btYellow
                state:"round"
                buttonText: qsTr("Yellow")
                phil: "yellow"

                onActivated: manager.yellowButtonPress()
            }
            StyledButton {
                id: btBlue
                state:"round"
                buttonText: qsTr("Blue")
                phil: "blue"
                onActivated: manager.blueButtonPress()
            }
        }

    }

}
