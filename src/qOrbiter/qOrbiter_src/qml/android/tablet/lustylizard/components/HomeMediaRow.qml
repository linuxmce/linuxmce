import QtQuick 1.0

Rectangle {
    id: btHomeMedia
    height: parent.height
    width: parent.height
    color:"transparent"


    Image {
        id: onimg
        source: "../images/btMedia.png"
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



    /*
Rectangle{
    id: btHomeMedia
    clip:false
    color:"transparent"

    HomeButtonDelegate{id:mediaDelegate}
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
*/

