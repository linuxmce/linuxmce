import QtQuick 1.0
import com.nokia.android 1.1


Rectangle{
    id:lightingrow
    height: scaleY(16)
    width: scaleX(100)
    //    transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 15 }
    clip:true
    color:"transparent"
    radius: 20

    //HomeButtonDelegate{id:lightingdelegate}
    Button{
        id:lightingdelegate
        text: title
        height: parent.height
        width: scaleX(15)

    }

    Row {
        id: guide
        spacing:5
        x: scaleX(2)

        Rectangle {
            id: lightingfloorplan
            height: style.stdbuttonh
            width: style.stdbuttonw
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            color:"transparent"


            Image {
                id: onimg
                source: "../img/ui3/lightingbig.png"
                height: parent.height
                width: parent.width
            }

            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked: {
                    manager.showfloorplan(2)
                    manager.setFloorplanType(2)
                }
            }
        }

            ListView{
                id: lightingScenarios
                width: stage.width
                height: scaleY(style.stdbuttonh)
                model: currentRoomLights
                spacing: 5
                orientation:ListView.Horizontal
                delegate:  lightingdelegate
                interactive: true
                anchors.left:lightingfloorplan.left

            }

    }
}


