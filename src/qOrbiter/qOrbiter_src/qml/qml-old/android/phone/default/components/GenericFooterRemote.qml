import QtQuick 1.1
import "../"
Item {
    id:genericBasicControls
    width: manager.appWidth
    height:appStyle.toolbarButtonHeight

    Rectangle{
        anchors.fill: parent
        color:appStyle.toolbarBgColor
    }

    Row{
        spacing: scaleX(2)
        anchors.verticalCenter: parent.verticalCenter
        anchors{
            left:parent.left
            right:parent.right
        }
        height: parent.height

        StyledButton {
            id:back
            buttonText: "back"
            anchors.verticalCenter: parent.verticalCenter
            onActivated: {manager.newTrack("-1")}
        }

        StyledButton {
            id:rewind
            buttonText: "RW"
            anchors.verticalCenter: parent.verticalCenter
            onActivated: {
                manager.setPlaybackSpeed(-2)
            }
        }

        StyledButton {
            id:play ;
            anchors.verticalCenter: parent.verticalCenter
            buttonText:dceTimecode.playbackSpeed !==0 ? qsTr("Pause") : qsTr("Play")
            onActivated: manager.pauseMedia()

        }

        StyledButton {
            id:ff ;
            buttonText: "FF"
            anchors.verticalCenter: parent.verticalCenter
            onActivated: manager.setPlaybackSpeed(2)
        }

        StyledButton {
            id:next ;
            buttonText: "Next";
            anchors.verticalCenter: parent.verticalCenter
            onActivated: manager.changedPlaylistPosition("+1")
        }

        StyledButton {
            id: stop
            buttonText: "stop"
            anchors.verticalCenter: parent.verticalCenter
            onActivated: manager.stopMedia()
        }
    }

}
