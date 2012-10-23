import QtQuick 2.0


Rectangle {
    id:gridholder
    width: scaleX(85)
    height: scaleY(85)
    clip: true
    anchors.horizontalCenter: parent.horizontalCenter
    color: "black"

    GridView {
        id: gridView
        //z: 2
        width: scaleX(82)
        height: scaleY(82)
        anchors.centerIn: gridholder
        model:dataModel
        delegate: contactDelegate
        focus: true
        //clip: true
        //contentItem.clip: true
        cellWidth: scaleX(20)
        cellHeight: scaleY(20)
        opacity:1
        scale:1
    }




}
