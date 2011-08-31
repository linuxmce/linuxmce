import QtQuick 1.0
//import "qrc:/components/";

Rectangle {
    id: rectangle1
    width: style.orbiterW
    height: style.orbiterH
    color: "transparent"
    // signal setupStart(int x, string y)
    Rectangle {
        width: style.orbiterW
        height: 75
        opacity: 1
        color: "transparent"
        Text {
            id: welcome
            text: "Welcome To LinuxMCE!"
            font.pixelSize: 30
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    Image {
        id: splash
        anchors.centerIn: rectangle1
        fillMode: Image.PreserveAspectFit
        source: "/img/splash.jpg"
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
          //  onTextChanged: setRouterIp(routerip.text)
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
            //onTextChanged: setDeviceNo(devicenumber.text)
            fillColor: style.button_system_color_hover
            anchors.verticalCenter: parent.verticalCenter

        }

        Rectangle {
            id: buttonsq1
            height: buttonLabel.height + 4;
            width: buttonLabel.width + 8
                color: style.button_system_color
                border.width: 1
                border.color: style.button_action_color
                radius: 4
                smooth:  true
                opacity: 1

            Text {
                id: buttonLabel
                text:"Connect"
                font.pointSize: 12
                anchors.centerIn:parent
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap

                 }

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: buttonaq1.color=style.button_system_color_hover
                onExited:  buttonsq1.color=style.button_system_color
                onClicked: setupStart(devicenumber.text, routerip.text)
            }
            //textSize: 12
            //text: "Connect"
            //radius:  4
            //onClicked: setupStart(devicenumber.text, routerip.text)
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
