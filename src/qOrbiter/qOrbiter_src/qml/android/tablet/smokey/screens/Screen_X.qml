import QtQuick 1.0
import "../components"
Item{
    id:screen_x_root
    anchors.fill: parent

    Rectangle{
        id:mask
        width: parent.width*85
        height: scaleY(15)
        color: "black"
        opacity: .65
        anchors.centerIn: parent
    }

    StyledText {
        id: screen_loading_error
        text: qsTr("I Cannot load this screen at this time. Please ensure that it exists and is error free.")
        anchors.centerIn: mask
        color: "Red"
        font.pixelSize: 32
        font.bold: false
    }
}
