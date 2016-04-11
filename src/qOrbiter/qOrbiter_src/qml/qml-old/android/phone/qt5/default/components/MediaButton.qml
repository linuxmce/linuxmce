import QtQuick 2.2

Rectangle {
    property alias media_but_img: media_button_image.source
    property alias media_but_txt: media_button_text.text
    id: rectangle1
    color:transparent
    height: style.stdbuttonh
    width: style.stdbuttonw

    Image {
        id: media_button_image
        fillMode: Image.PreserveAspectFit
        source: "../img/chkbxempty.png"
        anchors.centerIn: parent

    }

    Text {
        id: media_button_text
        text: "text"
        style: Text.Normal
        font.pixelSize: 10
        font.family: "Droid Sans"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

}
