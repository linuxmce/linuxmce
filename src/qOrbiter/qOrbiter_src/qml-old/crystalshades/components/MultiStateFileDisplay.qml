import QtQuick 1.1


Rectangle {
    id:gridholder
    width: scaleX(65) //RayBe was 85
    height: scaleY(65) //RayBe was 85
    clip: true
    anchors.horizontalCenter: parent.horizontalCenter
    color: "transparent"



    Image {
        id: name
        source: ""
    }

    GridView {
        id: gridView
        //z: 2
        width: scaleX(60) //RayBe was 82
        height: scaleY(60) //RayBe was 82
        anchors.centerIn: gridholder
        model: manager.getDataGridModel("MediaFile", 63)
        delegate: contactDelegate
        cacheBuffer: 20
        focus: true
       //clip: true
        //contentItem.clip: true
        cellWidth: scaleX(20)
        cellHeight: scaleY(20)


    }


}
