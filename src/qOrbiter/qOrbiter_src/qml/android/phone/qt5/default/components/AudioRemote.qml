import QtQuick 2.2
import "../../../skins-common/lib/handlers"

Rectangle {
    width: manager.appWidth
    height: scaleY(12)
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
            MouseArea{
                property string trackDirection:"-1" /*! takes value of +1 or -1  */
                anchors.fill: parent
                onClicked: manager.newTrack(trackDirection)
            }

        }

        MediaButton {id:rw ;
            color: "transparent"
            media_but_txt: "RW"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                property int speed:-2
                anchors.fill: parent
                onClicked: manager.setPlaybackSpeed(speed)
            }
        }

        MediaButton {
            id:play ;
            color: "transparent"
            media_but_txt: "Play"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                anchors.fill: parent
                onClicked: manager.pauseMedia()
            }

        }

        MediaButton {id:ff ;
            color: "transparent"
            media_but_txt: "FF"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                property int speed:2
                anchors.fill: parent
                onClicked: manager.setPlaybackSpeed(speed)
            }

        }
        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: "Next";
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                property string trackDirection:"+1" /*! takes value of +1 or -1  */
                anchors.fill: parent
                onClicked: manager.newTrack(trackDirection)
            }
        }


        MediaButton {
            id: stop
            color: "transparent"
            media_but_txt: "stop"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea
            {
                anchors.fill: parent
                onClicked: manager.stopMedia()
            }

        }
    }
}
