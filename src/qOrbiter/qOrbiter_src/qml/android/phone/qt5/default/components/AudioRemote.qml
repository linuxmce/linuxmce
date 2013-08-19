import QtQuick 2.0
import "../../lib/handlers"

Rectangle {
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"

    Image {
        id: overlay
        source: "../img/widegreyshape.png"
        width: appW
        height: style.stdbuttonh
        anchors.centerIn: parent

         }

    Row
    {

        spacing: scaleX(1.5)
        width: overlay.width
        height: overlay.height
        anchors.verticalCenter: overlay.verticalCenter
        anchors.horizontalCenter: overlay.horizontalCenter

        MediaButton {
            id:back
            color: "transparent"
            media_but_txt: "back"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
ChangeTrackHandler{
    trackDirection: "-1"
}

        }

        MediaButton {id:rw ;
            color: "transparent"
            media_but_txt: "RW"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            PlaybackSpeedHandler {
                speed:-2
            }
        }

        MediaButton {
            id:play ;
            color: "transparent"
            media_but_txt: "Play"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            PauseButtonHandler {
            }
        }

        MediaButton {id:ff ;
            color: "transparent"
            media_but_txt: "FF"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
           PlaybackSpeedHandler{
               speed:2
           }

        }
        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: "Next";
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            ChangeTrackHandler {
                trackDirection: "+1"
            }
        }


        MediaButton {
            id: stop
            color: "transparent"
            media_but_txt: "stop"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            StopMediaHandler {
            }
        }
    }
}
