import QtQuick 1.1

Rectangle{
    id:dragHandle
    height: 30
    width: 30
    radius: 30
    color:"grey"
    smooth:true
    anchors.verticalCenter: parent.verticalCenter
    MouseArea{
        anchors.fill: parent
        drag.target: parent
        onPressed: inactivity_timer.stop()
        onReleased: inactivity_timer.start()
        drag.axis: Drag.XAxis
        drag.maximumX: dragbar.width - (parent.radius /2)
        drag.minimumX: dragbar.pos.x -( parent.radius /2)
    }
}
