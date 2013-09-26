import QtQuick 1.0

Item {
    width: childrenRect.width
    height: childrenRect.height

    Rectangle{
        anchors.fill: parent
        color:"red"
        opacity: .25
    }

    Row{
        spacing: 5
        height: childrenRect.height
        width: childrenRect.width
        StyledButton {
            id:back
            buttonText:qsTr("Back")
            onActivated: manager.newTrack("-1")
        }
        StyledButton {
            id:rw
            buttonText:qsTr("Rewind")
            onActivated: manager.setPlaybackSpeed(-2)
        }
        StyledButton {
            id:play_pause
            buttonText:qsTr("Play")
            onActivated:{
                buttonText ==="Play" ? manager.mythTvPlay() :manager.pauseMedia()
                buttonText ==="Play" ? buttonText = qsTr("Pause") : buttonText = qsTr("Play")
            }
        }
        StyledButton {
            id:ff
            buttonText:qsTr("Fast Forward")
            onActivated: manager.setPlaybackSpeed(+2)
        }
        StyledButton {
            id:next
            buttonText:qsTr("Next")
            onActivated: manager.newTrack("+1")
        }
        StyledButton {
            id:stop
            buttonText:qsTr("Stop")
            onActivated:  manager.stopMedia()
        }

    }
}
