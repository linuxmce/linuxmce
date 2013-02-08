import QtQuick 1.0


Rectangle{
    id:titlerect
    height: childrenRect.height + 5
    width: parent.width
    color:"transparent"

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
        color: skinStyle.lighttext
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
