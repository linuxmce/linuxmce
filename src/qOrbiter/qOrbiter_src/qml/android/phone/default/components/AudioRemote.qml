import QtQuick 1.0


Rectangle {
    width: appW
    height: 80
    color: "transparent"

    Image {
        id: overlay
        source: "../img/widegreyshape.png"
        width: appW
        height: 76
        anchors.centerIn: parent

         }

    Row
    {
        x: 0
        y: 0
        spacing: 8
        width: overlay.width
        height: overlay.height
        anchors.left: overlay.left
        anchors.leftMargin: 0

        MediaButton {
            id:back
            color: "transparent"
            media_but_txt: "back"
            height: 45
            width: 45
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent               
                onClicked: dcerouter.changedTrack("-1")
            }
        }

        MediaButton {id:rw ;
            color: "transparent"
            media_but_txt: "RW"
            height: 45
            width: 45
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent                
                onClicked: rw_media(-2)
            }
        }

        MediaButton {
            id:play ;
            color: "transparent"
            media_but_txt: "Play"
            height: 45
            width: 45
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                anchors.fill: parent                
                onClicked: pauseMedia()
            }
        }

        MediaButton {id:ff ;
            color: "transparent"
            media_but_txt: "FF"
            height: 45
            width: 45
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent                 
                onClicked: ff_media(2)
            }

        }
        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: "Next";
            height: 45
            width: 45
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent                
                onClicked: dcerouter.changedTrack("+1")
            }
        }


        MediaButton {
            id: stop
            color: "transparent"
            media_but_txt: "stop"
            height: 45
            width: 45
            anchors.verticalCenter: parent.verticalCenter
            MouseArea
            {
                anchors.fill: parent                 
                onClicked: stopMedia()
            }
        }
    }
}
