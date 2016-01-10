import QtQuick 1.1
import "../../components"

Rectangle {
    id: rectangle1
    width: style.orbiterW
    height: style.orbiterH
    color: "transparent"
    // signal setupStart(int x, string y)

    Image {
        id: splash
        anchors.centerIn: rectangle1
        fillMode: Image.PreserveAspectFit
        source: "../../../../img/lmcesplash.png"
        anchors.fill: parent
    }
    Rectangle {
        width: style.orbiterW
        height: 75
        color: "transparent"
        Text {
            id: welcome
            text: "Welcome To LinuxMCE!"
            font.pixelSize: 30
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
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
        text: "Set Connection Details"
        font.bold: true
        anchors.top: rectangle2.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row{
        anchors.centerIn: rectangle2
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        Text {
            text:  "Host:"
            anchors.verticalCenter: parent.verticalCenter
        }

        TextInput {
            id: routerip
            width: 110
            text: srouterip
            font.pixelSize: 12
            fillColor: style.button_system_color_hover
            anchors.verticalCenter: parent.verticalCenter

        }
        Text {
            text:  "Device:"
            anchors.verticalCenter: parent.verticalCenter
        }
        TextInput {
            id: devicenumber
            width: 25
            text: deviceid
            font.pixelSize: 12
            fillColor: style.button_system_color_hover
            anchors.verticalCenter: parent.verticalCenter

        }

        Button {
            id: buttonsq1
            textSize: 12
            text: "Connect"
            radius:  4
            onClicked: {

                var val = qmlSetupLmce(deviceid, srouterip)
                console.log("Connect returned:",val)
            }
            anchors.verticalCenter: parent.verticalCenter
        }
    }
 //Component.onCompleted: qmlSetupLmce(deviceid, srouterip)
}
