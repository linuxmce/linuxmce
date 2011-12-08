/*
  Splash.qml This is the general linuxmce splash screen and should be used for all devices on startup.
  */
import QtQuick 1.0

Rectangle {
    id: rectangle1
    width: 800
    height: 600
    color: "slategrey"
    signal setupStart(string x, string y)



    Rectangle {
        width: parent.width
        height: 75
        opacity: 1
        color: "transparent"
        Text {
            id: welcome
            text: qsTr("Welcome To LinuxMCE!")
            font.pixelSize: 30
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

        }

    }
    Image {
        id: splash
        anchors.centerIn: rectangle1
        fillMode: Image.PreserveAspectFit
        source: "qrc:/img/Splash.png"
        anchors.fill: parent
    }    


}
