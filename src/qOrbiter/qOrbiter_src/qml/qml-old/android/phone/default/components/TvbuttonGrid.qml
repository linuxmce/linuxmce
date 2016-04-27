import QtQuick 1.0
import "../components"

Item {
    id: remote1
    height:parent.height
    width: parent.width
    property int rowHeight:scaleY(12)
    Column { // Info, Up, Menu, Ch+
        id:leftCol
        spacing: manager.isProfile ? scaleX(1) : scaleY(1)
        height: parent.height
        width: scaleX(18)
        anchors{
            left:parent.left
            top:parent.top
        }

        StyledButton {
            state:"fixed"
            id: btVolUp
            buttonText: "Vol+"
            onActivated: manager.adjustVolume(+1)
        }
        StyledButton {
            id: btVolDown
            state:"fixed"
            buttonText: "Vol-"
            onActivated: manager.adjustVolume(-1)
        }

        StyledButton {
            id: off_btn
            state:"fixed"
            buttonText: "Off"
            onActivated: manager.stopMedia()
        }

    }

    Column { // Info, Up, Menu, Ch+
        id:rightCol
        spacing: manager.isProfile ? scaleX(1) : scaleY(1)
        height: parent.height
        width: scaleX(18)
        anchors{
            right:parent.right
            top:parent.top
        }

        StyledButton {
            id: btChannelUp
            state:"fixed"
            buttonText: "Ch+"
            onActivated: manager.changeTrack("+1")
        }
        StyledButton {
            id: btSkipNext
            state:"fixed"
            buttonText: "Mute"
            onActivated: manager.muteSound()
        }
        StyledButton {
            state:"fixed"
            id: btChannelDown
            buttonText: "Ch-"
            onActivated: manager.changeTrack("-1")
        }


    }

    Rectangle{
        id:mainDiamond
        height: scaleY(12)*3
        width:  scaleX(18)*3
        color:"transparent"
       anchors{
        horizontalCenter: parent.horizontalCenter
        top:parent.top
       }
        StyledButton {
            id: btUp
            anchors{
                top:parent.top
                horizontalCenter: parent.horizontalCenter

            }
            state:"fixed"
            buttonText: "Up"
            onActivated: manager.moveArrowDirection(1) //up
        }

        StyledButton {
            state:"fixed"
            anchors{
                bottom:parent.bottom
                 horizontalCenter: parent.horizontalCenter
            }
            id: btDown
            buttonText: "Down"
            onActivated: manager.moveArrowDirection(2) //down
        }
        StyledButton {
            id: btLeft
            anchors{
                left:parent.left
                verticalCenter: parent.verticalCenter
            }
            state:"fixed"
            buttonText: "Left"
            onActivated: manager.moveArrowDirection(3)
        }
        StyledButton {
            id: btOk
            anchors{
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }
            state:"fixed"
            buttonText: "Ok"
            onActivated: manager.moveArrowDirection(5) //ok
        }
        StyledButton {
            id: btRight
            anchors{
                right:parent.right
                verticalCenter: parent.verticalCenter
            }
            state:"fixed"
            buttonText: "Right"
            onActivated: manager.moveArrowDirection(4)
        }

    }


    Row { // Exit, Down, Guide, VolUp
        id:dvrRow
        spacing: manager.isProfile ? scaleX(1) : scaleY(1)
        height: rowHeight
        anchors{
            bottom:transports.top
            left:parent.left
            right:parent.right
        }
        StyledButton {
            state:"fixed"
            id: btExit
            buttonText: "Exit"
            onActivated: manager.extraButton("exit")
        }

        StyledButton {
            state:"fixed"
            id: btGuide
            buttonText: "Guide"
            onActivated: manager.extraButton("guide")
        }
        StyledButton {
            id: btInfo
            state:"fixed"
            buttonText: "Info"
            onActivated: manager.extraButton("info")
        }

        StyledButton {
            id: btMenu
            state:"fixed"
            buttonText: "Menu"
            onActivated: manager.extraButton("menu")
        }

    }


    Row {
        id:transports// SkipBack, Rewind, FastForward, SkipFwd

        spacing: manager.isProfile ? scaleX(1) : scaleY(1)

        height: rowHeight
        anchors{
            left:parent.left
            right:parent.right
            bottom:colorButtons.top
        }
        StyledButton {
            id: btStop
            state:"fixed"
            buttonText: "Stop"
            // This is the Live TV remote, so we use the "Stop" CMD (AV control), and not the "Stop Media" CMD (DCE players)
            onActivated: manager.stop_AV()
        }
        StyledButton {
            id: btRewind
            state:"fixed"
            buttonText: "Rewind"
            onActivated: manager.setPlaybackSpeed(-1)
        }
        StyledButton {
            id: btPause
            state:"fixed"
            buttonText: "Pause"
            onActivated: manager.pause()
        }
        StyledButton {
            id: btPlay
            state:"fixed"
            buttonText: "Play"
            onActivated: manager.play()
        }
        StyledButton {
            id: btFFwd
            state:"fixed"
            buttonText: "FFwd"
            onActivated: manager.setPlaybackSpeed(1)
        }

    }

    Row { // Red, Green, Yellow, Blue
        id:colorButtons
        spacing: manager.isProfile ? scaleX(1) : scaleY(1)
        height: rowHeight
        anchors{
            bottom:parent.bottom
            left:parent.left
            right:parent.right
        }
        StyledButton {
            id: btRed
            state:"fixed"
            buttonText: "Red"
            phil: "red"

            onActivated: manager.redButtonPress()
        }
        StyledButton {
            id: btGreen
            state:"fixed"
            buttonText: "Green"
            phil: "green"

            onActivated: manager.greenButton()
        }
        StyledButton {
            id: btYellow
            state:"fixed"
            buttonText: "Yellow"
            phil: "yellow"

            onActivated: manager.yellowButton()
        }
        StyledButton {
            id: btBlue
            state:"fixed"
            buttonText: "Blue"
            phil: "blue"
            onActivated: manager.blueButton()
        }
    }

}
