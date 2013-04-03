import QtQuick 1.1

Rectangle {

    property alias media_but_txt: media_button_text.text
    property alias button:handler
    id: rectangle1
    height: skinStyle.mediabuttonH
    width: skinStyle.mediabuttonW
    border.color: "white"
    border.width: 1
    color: handler.pressed ? "yellow" : "green"
    Image {
        id: media_button_image
        source: manager.imagePath+"ui3/rounded_button.png"
        anchors.fill: parent
        opacity: .35
    }

    StyledText {
        id: media_button_text
        isBold: true
        text: "text"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
    MouseArea{
        id:handler
        anchors.fill: parent
    }

}
