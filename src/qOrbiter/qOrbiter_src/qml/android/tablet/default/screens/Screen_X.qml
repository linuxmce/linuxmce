import QtQuick 1.0
import "../components"
StyledScreen{
    id:screen_x_root


    StyledText {
        id: screen_loading_error
        text: qsTr("I Cannot load this screen at this time. Please ensure that it exists and is error free.")
        anchors.centerIn: mask
        color: "Red"
        font.pixelSize: 32
        font.bold: false
    }

}
