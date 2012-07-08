import QtQuick 1.0

Rectangle {
    id: rectangle1
    width: 800
    height: 480

    Text {
        id: text1
        x: 136
        y: 209
        text: "Welcome To Linux MCE!"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 30
    }
    Image {
        id: splash
        source: "../../../img/lmcesplash.jpg"
        height: 480
        width: 800
        anchors.centerIn: parent


    }

}
