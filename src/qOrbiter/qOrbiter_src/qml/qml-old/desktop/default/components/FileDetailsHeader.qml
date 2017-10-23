import QtQuick 1.1


Item{
    id:titlerect
    anchors.fill: parent

    Rectangle{
        color:"black"
        opacity: .25
        anchors.fill: titlerect
    }
    StyledText {
        id: text2
        anchors.left: parent.left
        anchors.leftMargin: scaleX(1)
        text: qsTr("Filename: ")+filedetailsclass.filename
        font.pixelSize: 14
        font.bold: true
        color: skinstyle.lighttext
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

    StyledText {
        id: storageDeviceText
        text: qsTr("Located on storage device: ") + filedetailsclass.qs_storageDevice
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        smooth: true
        font.pixelSize: scaleY(2)
        anchors.right: titlerect.right
    }
}
