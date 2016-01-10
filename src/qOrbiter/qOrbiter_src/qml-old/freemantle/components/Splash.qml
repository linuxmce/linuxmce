import QtQuick 1.0
import "../components"

Rectangle {
    id: rectangle1
    width: 800
    height: 800
   // signal setupStart(int x, string y)
    Text {
        id: text2
        x: 160
        y: 404
        text: "text"
        font.pixelSize: 12

        Text {
            id: welcome
            x: 50
            y: -396
            text: "Welcome To Linux MCE!"
            anchors.verticalCenterOffset: -386
            anchors.horizontalCenterOffset: 216
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 30
        }
    }

    Image {
        id: splash
        x: -6
        y: 0
        source: "../../../img/lmcesplash.jpg"
        height: 800
        width: 812
        anchors.centerIn: parent

        Rectangle {
            id: rectangle2
            x: 145
            y: 374
            width: 457
            height: 77
            radius: 7
            border.width: 1
            border.color: "#000000"
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#ffffff"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
            opacity: 0.3
        }

        TextEdit {
            id: routerip
            x: 150
            y: 403
            width: 80
            height: 20
            text: srouterip
            cursorVisible: true
            font.pixelSize: 12
        }

        TextEdit {
            id: devicenumber
            x: 516
            y: 403
            width: 80
            height: 20
            text: deviceid
            cursorVisible: true
            font.pixelSize: 12
        }

        ButtonSq {
            id: buttonsq1
            height: 43
            buttontextfontsize: 14
            buttonsqradius: 5
            width: 78
            buttontext: "Connect"
            x: 335
            y: 392

            MouseArea{
                anchors.fill: parent
                onClicked: setupStart(deviceid, srouterip)
            }
        }




    }




}
