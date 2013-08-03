import QtQuick 1.0

Item {
    width: childrenRect.width
    height: childrenRect.height

    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .65
    }

    Row{
        spacing: 5
        height: childrenRect.height
        StyledButton{
            buttonText.text: "Back"
            hitArea.onReleased: manager.newTrack("-1")
        }

        StyledButton{
            buttonText.text: "RW"
            hitArea.onReleased: manager.setPlaybackSpeed(-1)
        }

        StyledButton{
            buttonText.text:dceTimecode.playbackSpeed === 0 ? "Play" : "Pause"
            hitArea.onReleased: manager.pauseMedia()
        }

        StyledButton{
            buttonText.text: "FF"
            hitArea.onReleased: manager.setPlaybackSpeed(+2)
        }

        StyledButton{
            buttonText.text: "Next"
            hitArea.onReleased: manager.newTrack("+1")
        }

        StyledButton{
            buttonText.text: "Stop"
            hitArea.onReleased: manager.stopMedia()
        }
    }
}
