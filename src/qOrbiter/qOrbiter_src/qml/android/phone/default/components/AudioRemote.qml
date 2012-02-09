import QtQuick 1.0


Rectangle {
    width: 280
    height: 80
    color: "transparent"

    Image {
        id: overlay
        source: "../img/widegreyshape.png"
        width: 285
        height: 76
        anchors.centerIn: parent

         }

    Row
    {
        spacing: 22
        width: overlay.width
        height: overlay.height
        anchors.left: overlay.left
        anchors.leftMargin: 10

        MediaButton {
            id:back
            color: "transparent"
            media_but_txt: "back"
            height: 25
            width: 25
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: back.scale = .75
                onExited: back.scale = 1
                onClicked: dcerouter.changedTrack("-1")
            }
        }

        MediaButton {id:rw ;
            color: "transparent"
            media_but_txt: "RW"
            height: 25
            width: 25
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: rw_media(-2)
            }
        }

        MediaButton {
            id:play ;
            color: "transparent"
            media_but_txt: "Play"
            height: 25
            width: 25
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: pauseMedia()
            }
        }

        MediaButton {id:ff ;
            color: "transparent"
            media_but_txt: "FF"
            height: 25
            width: 25
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: ff_media(2)
            }

        }
        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: "Next";
            height: 25
            width: 25
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: dcerouter.changedTrack("+1")
            }
        }


        MediaButton {
            id: stop
            color: "transparent"
            media_but_txt: "stop"
            height: 25
            width: 25
            anchors.verticalCenter: parent.verticalCenter
            MouseArea
            {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: stopMedia()
            }
        }
    }
}
