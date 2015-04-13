import QtQuick 2.2

Rectangle {
    width: manager.appWidth
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

        spacing: scaleX(1.5)
        width: overlay.width
        height: overlay.height
        anchors.verticalCenter: overlay.verticalCenter
        anchors.horizontalCenter: overlay.horizontalCenter

        MediaButton {
            id:up
            color: "transparent"
            media_but_txt: "back"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: manager.changedTrack("-1")
            }
        }

        MediaButton {
            id:dwn ;
            color: "transparent"
            media_but_txt: "RW"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: manager.RwMedia(-2)
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
                onClicked: manager.PauseMedia()
            }
        }

        MediaButton {id:ff ;
            color: "transparent"
            media_but_txt: "FF"
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: manager.FfMedia(+2)
            }

        }
        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: "Next";
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: manager.changedTrack("+1")
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
                onClicked: manager.StopMedia()
            }
        }
    }
}
