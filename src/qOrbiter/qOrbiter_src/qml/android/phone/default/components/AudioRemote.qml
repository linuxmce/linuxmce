import QtQuick 1.0
import "../../../skins-common/lib/handlers"

Rectangle {
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"

    Image {
        id: overlay
        source: "../img/widegreyshape.png"
        width: manager.appWidth
        height: style.stdbuttonh
        anchors.centerIn: parent

    }

    Row
    {
        spacing: scaleX(2)
        width: manager.appWidth
        height: overlay.height
        anchors.verticalCenter: overlay.verticalCenter
        anchors.horizontalCenter: overlay.horizontalCenter

        MediaButton {
            id:back
            color: "transparent"
            media_but_txt: "back"

            anchors.verticalCenter: parent.verticalCenter
            ChangeTrackHandler{
                trackDirection: "-1"
            }

        }

        MediaButton {id:rw ;
            color: "transparent"
            media_but_txt: "RW"

            anchors.verticalCenter: parent.verticalCenter
            PlaybackSpeedHandler {
                speed:-2
            }
        }

        MediaButton {
            id:play ;
            color: "transparent"
            media_but_txt: "Play"

            anchors.verticalCenter: parent.verticalCenter
            PauseButtonHandler {
            }
        }

        MediaButton {id:ff ;
            color: "transparent"
            media_but_txt: "FF"

            anchors.verticalCenter: parent.verticalCenter
            PlaybackSpeedHandler{
                speed:2
            }

        }
        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: "Next";

            anchors.verticalCenter: parent.verticalCenter
            ChangeTrackHandler {
                trackDirection: "+1"
            }
        }


        MediaButton {
            id: stop
            color: "transparent"
            media_but_txt: "stop"

            anchors.verticalCenter: parent.verticalCenter
            StopMediaHandler {
            }
        }
    }
}
