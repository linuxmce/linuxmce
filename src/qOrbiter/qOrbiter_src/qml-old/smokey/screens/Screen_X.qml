import QtQuick 1.0
import "../components"
Item{
    id:screen_x_root
   height: manager.appHeight
   width: manager.appWidth

    Rectangle{
        id:mask
        width: parent.width*85
        height: scaleY(15)
        color: "black"
        opacity: .85
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
