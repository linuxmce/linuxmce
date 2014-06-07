import QtQuick 1.0
import "../components"

Item {
    id: remote1

    height:parent.height
    width: parent.width
    property int rowHeight:scaleY(8)
    Column { // Info, Up, Menu, Ch+
        id:col1
        spacing: 1
        height: parent.height
        width: parent.width / 3
        anchors{
            left:parent.left
            top:parent.top
        }

        Row {
            spacing: 1
            height: rowHeight
            anchors{
                left:parent.left
                right:parent.right
            }

            StyledButton {
                id: btInfo
                buttonText: "Info"
                onActivated: manager.extraButton("info")
            }
            StyledButton {
                id: btUp
                buttonText: "Up"
                onActivated: manager.moveArrowDirection(1) //up
            }
            StyledButton {
                id: btMenu
                buttonText: "Menu"
                onActivated: manager.extraButton("menu")
            }
            StyledButton {
                id: btChannelUp
                buttonText: "Ch+"
                onActivated: manager.changeTrack("+1")
            }
        }
        Row {
            spacing: 1
            height: rowHeight
            anchors{
                left:parent.left
                right:parent.right
            }
            StyledButton {
                id: btLeft
                buttonText: "Left"
                onActivated: manager.moveArrowDirection(3)
            }
            StyledButton {
                id: btOk
                buttonText: "Ok"
                onActivated: manager.moveArrowDirection(5) //ok
            }
            StyledButton {
                id: btRight
                buttonText: "Right"
                onActivated: manager.moveArrowDirection(4)
            }
            StyledButton {
                id: btChannelDown
                buttonText: "Ch-"
                onActivated: manager.changeTrack("-1")
            }
        }
        Row { // Exit, Down, Guide, VolUp
            spacing: 1
            height: rowHeight
            anchors{
                left:parent.left
                right:parent.right
            }
            StyledButton {
                id: btExit
                buttonText: "Exit"
                onActivated: manager.extraButton("exit")
            }
            StyledButton {
                id: btDown
                buttonText: "Down"
                onActivated: manager.moveArrowDirection(2) //down
            }
            StyledButton {
                id: btGuide
                buttonText: "Guide"
                onActivated: manager.extraButton("guide")
            }
            StyledButton {
                id: btVolUp
                buttonText: "Vol+"
                onActivated: manager.adjustVolume(+1)
            }
        }
        Row { // Stop, Pause, Play, VolDown
            height: rowHeight
            anchors{
                left:parent.left
                right:parent.right
            }
            StyledButton {
                id: btStop
                buttonText: "Stop"
                // This is the Live TV remote, so we use the "Stop" CMD (AV control), and not the "Stop Media" CMD (DCE players)
                onActivated: manager.stop_AV()
            }
            StyledButton {
                id: btPause
                buttonText: "Pause"
                onActivated: manager.pause()
            }
            StyledButton {
                id: btPlay
                buttonText: "Play"
                onActivated: manager.play()
            }
            StyledButton {
                id: btVolDown
                buttonText: "Vol-"
                onActivated: manager.adjustVolume(-1)
            }
        }
        
        Row { // SkipBack, Rewind, FastForward, SkipFwd
            height: rowHeight
            anchors{
                left:parent.left
                right:parent.right
            }
            StyledButton {
                id: btSkipBack
                buttonText: "Off"
                onActivated: manager.stopMedia()
            }
            StyledButton {
                id: btRewind
                buttonText: "Rewind"
                onActivated: manager.setPlaybackSpeed(-1)
            }
            StyledButton {
                id: btFFwd
                buttonText: "FFwd"
                onActivated: manager.setPlaybackSpeed(1)
            }
            StyledButton {
                id: btSkipNext
                buttonText: "Mute"
                onActivated: manager.muteSound()
            }
        }
        
        Row { // Red, Green, Yellow, Blue
            height: rowHeight
            anchors{
                left:parent.left
                right:parent.right
            }
            StyledButton {
                id: btRed
                buttonText: "Red"
                phil: "red"
                
                onActivated: manager.redButtonPress()
            }
            StyledButton {
                id: btGreen
                buttonText: "Green"
                phil: "green"
                
                onActivated: manager.greenButton()
            }
            StyledButton {
                id: btYellow
                buttonText: "Yellow"
                phil: "yellow"
                
                onActivated: manager.yellowButton()
            }
            StyledButton {
                id: btBlue
                buttonText: "Blue"
                phil: "blue"
                
                onActivated: manager.blueButton()
            }
        }
    }
}
