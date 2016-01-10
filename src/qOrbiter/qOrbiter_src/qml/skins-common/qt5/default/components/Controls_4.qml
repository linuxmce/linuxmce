import QtQuick 2.2

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
            buttonText: "Back"
            hitArea.onReleased: manager.newTrack("-1")
        }

        StyledButton{
            buttonText: "RW"
            hitArea.onReleased: manager.setPlaybackSpeed(-1)
        }

        StyledButton{
            buttonText:dceTimecode.playbackSpeed === 0 ? "Play" : "Pause"
            hitArea.onReleased: manager.pauseMedia()
        }

        StyledButton{
            buttonText: "FF"
            hitArea.onReleased: manager.setPlaybackSpeed(+2)
        }

        StyledButton{
            buttonText: "Next"
            hitArea.onReleased: manager.newTrack("+1")
        }

        StyledButton{
            buttonText: "Stop"
            hitArea.onReleased: manager.stopMedia()
        }
    }
}
