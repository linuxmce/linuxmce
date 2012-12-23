import QtQuick 1.0


Rectangle{
    height: scaleY(15)
    width: scaleX(100)
    clip:false
    color:"transparent"

    HomeButtonDelegate{id:mediaDelegate}

    Row {
        id: guide
        spacing:5
        x: scaleX(2)

        Rectangle {
            id: rowheader
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            height: style.stdbuttonh
            width: style.stdbuttonw
            color: "transparent"
            radius: style.but_smooth

            Image {
                id: mHeaderImg
                source: "../img/ui3/mediabig.png"
                height: parent.height
                width: parent.width
            }

            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked:{
                    manager.setFloorplanType(3)
                    manager.ShowFloorPlan(3)

                }
            }
        }

        NowPlayingButton {
            id: now_playing
        }
        Flickable{
            id:mediaflick
            height: scaleY(16)
            width: scaleX(79)
            contentHeight: style.buttonH
            contentWidth: (style.buttonW + 5) * (mediaflick.children.width)
            clip: true
            flickableDirection: "HorizontalFlick"
            anchors.verticalCenter: parent.verticalCenter

            ListView{
                id: mediaScenarios
                height: scaleY(style.buttonH)
                width: stage.width

                model: currentRoomMedia
                orientation:ListView.Horizontal
                spacing:5
                delegate: mediaDelegate
                interactive: false

            }
        }
    }
}

