import QtQuick 1.0


Rectangle {
    id:gridholder
    width: scaleX(85)
    height: scaleY(85)
    clip: true
    anchors.horizontalCenter: parent.horizontalCenter
    color: "transparent"
    property alias maingrid:gridView


    GridView {
        id: gridView
        //z: 2
        width: scaleX(82)
        height: scaleY(80)
        anchors.centerIn: gridholder
        model:dataModel
        delegate: contactDelegate
        cacheBuffer: 0
        focus: true
        //clip: true
        contentItem.clip: true
        cellWidth: scaleX(25)
        cellHeight: scaleY(25)

        opacity:1
        scale:1
       // transform: Rotation { origin.x: width/2; origin.y: y/2; axis { x: 0; y: 1; z: 0 } angle: gridView.moving ? gridView.horizontalVelocity > 0 ? 20 : -20 :0 }
        flow:GridView.TopToBottom
        Behavior on transform {
            PropertyAnimation{
                duration: 1000
                }
        }

    } 


}
