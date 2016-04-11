import QtQuick 1.1



Rectangle {
    id:gridholder
    width: scaleX(85)
    height: scaleY(85)
    clip: true
    anchors.horizontalCenter: parent.horizontalCenter
    color: "transparent"

    GridView {
        id: gridView
        //z: 2
        width: scaleX(85)
        height: scaleY(85)
        anchors.centerIn: gridholder
        model:dataModel
        delegate: contactDelegate
        focus: true
        //clip: true
        //contentItem.clip: true
        cellWidth: scaleX(21)
        cellHeight: scaleY(21)

        opacity:1
        scale:1
        cacheBuffer: 0

    }

}
