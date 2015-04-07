import QtQuick 1.0

Rectangle {
    id: btHomeLighting
    height: parent.height
    width: parent.height
    color:"transparent"


    Image {
        id: onimg
        source: "../images/btLights.png"
        height: parent.height
        width: parent.width
    }

    MouseArea{
        id: mousearea1
        anchors.fill: parent
        onClicked: {
                manager.ShowFloorPlan(2)
            manager.setFloorplanType(2)
        }

    }
}

/*
    Rectangle{
        id:btHomeLighting
//    transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 15 }
        clip:true
        color:"transparent"
        radius: 20

        HomeButtonDelegate{id:lightingdelegate}

            Flickable{
                height: scaleY(16)
                width: scaleX(79)
                contentHeight: style.stdbuttonh
                contentWidth: ((style.stdbuttonw + 5) * (lightingScenarios.count + 1)) - 5
                clip: false
                flickableDirection: "HorizontalFlick"

                ListView{
                    id: lightingScenarios
                    width: stage.width
                    height: scaleY(style.stdbuttonh)
                    model: currentRoomLights
                    spacing: 5
                    orientation:ListView.Horizontal
                    delegate:  lightingdelegate
                    interactive: false

                }
            }
        }
    }
*/
