import QtQuick 1.0
Rectangle{
    id:btHomeLighting
//    transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 15 }
    clip:true
    color:"#000000"
    opacity: 0.800
    visible: false

    width: scaleX(97)
    height: 100
    anchors.bottom: mainButtonContainer.top
    anchors.bottomMargin:  16
    anchors.horizontalCenter: parent.horizontalCenter

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
