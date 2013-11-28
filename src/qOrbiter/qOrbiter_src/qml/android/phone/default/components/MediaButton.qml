import QtQuick 1.0

Item {
    property alias media_but_img: media_button_image.source
    property alias media_but_txt: media_button_text.text
    property string color:"transparent"

    id: buttonRect
    signal activated()
    height: scaleX(14)
    width: height

    Rectangle{
        anchors.fill: parent
        color:buttonRect.color
    }

    Image {
        id: media_button_image
        source: ""
        anchors.fill: parent
    }

    Text {
        id: media_button_text
        text: "text"
        style: Text.Normal
        font.pixelSize: 18
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea{
        anchors.fill: parent
        onReleased:activated()
    }

}
