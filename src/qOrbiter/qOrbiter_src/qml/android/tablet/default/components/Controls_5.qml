import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
Item {
    width: childrenRect.width
    height: childrenRect.height
    property bool useDvd:media_playback_base.state==="dvdmenu"
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

        StyledButton{
            buttonText: "+"
            hitArea.onReleased: manager.adjustVolume(5)
        }
        StyledButton{
            buttonText: "Mute"
            hitArea.onReleased: manager.mute()
        }
        StyledButton{
            buttonText: "-"
            hitArea.onReleased: manager.adjustVolume(-5)
        }
    }


}
