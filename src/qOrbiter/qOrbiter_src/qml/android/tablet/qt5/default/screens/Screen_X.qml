import QtQuick 2.0
import "../components"
Item{
    id:screen_x_root
    anchors.fill: parent
    StyledText {
        id: screen_loading_error
        text: qsTr("I Cannot load this screen at this time. Please ensure that it exists and is error free.")
        anchors.centerIn: parent
        color: "Red"
        font.pixelSize: 32
        font.bold: false
    }
}
