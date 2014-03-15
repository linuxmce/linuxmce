import QtQuick 1.0


Item {
    id:gridholder
    width: scaleX(85)
    height: scaleY(85)
    clip: true
    anchors.horizontalCenter: parent.horizontalCenter
    property alias maingrid:gridView
    GridView {
        id: gridView
        //z: 2
        width: parent.width
        height: parent.height
        anchors.centerIn: gridholder
        model: manager.getDataGridModel("MediaFile", 63)
        delegate: MediaListGridDelagate{}
        cacheBuffer: 15
        focus: true
        //clip: true
        contentItem.clip: true
        cellWidth: contactDelegate.width
        cellHeight: contactDelegate.height
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
