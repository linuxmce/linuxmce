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
        width: style.orbiterW
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
        Component.onCompleted: rectangle1.setupStart(devicenumber.text, routerip.text)
    }
    Image {
        id: splash
        anchors.centerIn: rectangle1
        fillMode: Image.PreserveAspectFit
        source: "qrc:/img/Splash.png"
        anchors.fill: parent
    }
    Rectangle {
        id: rectangle2
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        width: 457
        height: 57
        radius: 7
        anchors.centerIn: rectangle1
        border.width: 1
        border.color: "#000000"
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "transparent"
            }
            GradientStop {
                position: 1
                color: "#000000"
            }
        }
        opacity: 0.5
    }
    Text {
        id: connectionlabel
        text: qsTr("Set Connection Details")
        font.bold: true
        anchors.top: rectangle2.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row{
        anchors.centerIn: rectangle2
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        Text {
            text: qsTr("Host:")
            anchors.verticalCenter: parent.verticalCenter
        }

        TextInput {
            id: routerip
            width: 110
            text: srouterip
            font.pixelSize: 12
          //  onTextChanged: setRouterIp(routerip.text)
            fillColor: "aliceblue"
            anchors.verticalCenter: parent.verticalCenter


        }
        Text {
            text: qsTr("Device:")
            anchors.verticalCenter: parent.verticalCenter
        }
        TextInput {
            id: devicenumber
            width: 25
            text: deviceid
            font.pixelSize: 12
            //onTextChanged: setDeviceNo(devicenumber.text)
            fillColor: "aliceblue"
            anchors.verticalCenter: parent.verticalCenter

        }

     /* Button {
            id: buttonsq1
            textSize: 12
            text: qsTr("Connect")
            radius:  4
            onClicked: setupStart(devicenumber.text, routerip.text)
            anchors.verticalCenter: parent.verticalCenter

        } */

    }
}
