import QtQuick 2.0

Rectangle {
    property alias media_but_img: media_button_image.source
    property alias media_but_txt: media_button_text.text
    id: rectangle1
    height: style.mediabuttonH
    width: style.mediabuttonW
    Image {
        id: media_button_image
        source: ""
        anchors.centerIn: parent
    }
    radius: 25
    StyledText {
        id: media_button_text
        x: 0
        y: 26
        text: "text"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
}




}
