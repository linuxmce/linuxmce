import QtQuick 1.0


Rectangle {
    id:gridholder
    width: scaleX(85)
    height: scaleY(85)
    clip: true
    anchors.top: parent.top
    anchors.topMargin: scaleY(5)
    anchors.horizontalCenter: parent.horizontalCenter
    color: style.lighthighlight
    border.color: "aliceblue"
    border.width: 2


    Image {
        id: name
        source: ""
    }

    GridView {
        id: gridView
        //z: 2
        width: scaleX(82)
        height: scaleY(82)
        anchors.centerIn: gridholder
        model:dataModel
        delegate: contactDelegate
        highlight: Rectangle { color: "lightblue"; radius: 5 }
        highlightFollowsCurrentItem: true
        focus: true
       //clip: true
        //contentItem.clip: true
        cellWidth: scaleX(20)
        cellHeight: scaleY(20)

    }


}
