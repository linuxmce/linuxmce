import QtQuick 2.2
import "../components"
StyledScreen{
    id:screen_x_root


    Rectangle{
        height: scaleY(25)
        width: parent.width
        gradient: style.buttonGradient
        anchors.verticalCenter: parent.verticalCenter
        border.width:2
        border.color: "white"
        Rectangle{
            anchors.fill: parent
            color:"yellow"
            opacity: .65
        }

        StyledText {
            id: screen_loading_error
            text: qsTr("I Cannot load this screen at this time. Please ensure that it exists and is error free.")
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            color: "Black"
            font.pixelSize: 28
            font.bold: true
            font.weight: Font.Bold
        }
        StyledText{
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text:"Error:"+ pageLoader.errorString
        }
    }



}
