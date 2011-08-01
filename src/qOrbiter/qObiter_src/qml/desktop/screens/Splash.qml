import QtQuick 1.0
import "../components"

Rectangle {
    id: rectangle1
    width: style.orbiterW
    height: style.orbiterH
    color: style.bgcolor

   // signal setupStart(int x, string y)

        Text {
            id: welcome
            text: "Welcome To Linux MCE!"
            font.pixelSize: 30
            z:5
        }
        Text {
            id: connectionlabel
            text: "Set Connection Details"
            font.bold: true
            anchors.top: rectangle2.top
            anchors.horizontalCenter: rectangle2.horizontalCenter
            z:10
        }

        Row{
            anchors.centerIn: rectangle2
            height: childrenRect.height
            width: childrenRect.width
            z:5
            spacing: 50


            TextEdit {
                id: routerip
                width: 50
                height: 10
                text: srouterip
                cursorVisible: true
                font.pixelSize: 12
                onTextChanged: setRouterIp(routerip.text)
                fillColor: style.button_system_color_hover
                }

            TextEdit {
                id: devicenumber
                width: 50
                height: 10
                text: deviceid
                cursorVisible: true
                font.pixelSize: 12
                onTextChanged: setDeviceNo(devicenumber.text)
               fillColor: style.button_system_color_hover
                    }

            ButtonSq {
                id: buttonsq1
                height: 43
                buttontextfontsize: 14
                buttonsqradius: 5
                width: 78
                buttontext: "Connect"
                color: style.button_system_color_hover
                opacity: 1

                MouseArea{
                    anchors.fill: parent
                    onClicked: setupStart(deviceid, srouterip)
                          }
                     }
            }


    Image {
        id: splash
        anchors.centerIn: rectangle1
        fillMode: Image.PreserveAspectFit
        source: "../../../img/lmcesplash.jpg"
        anchors.fill: parent
             }

    Rectangle {
        id: rectangle2
        x: 145
        y: 374
        width: 457
        height: 77
        radius: 7
        anchors.centerIn: rectangle1
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

    }
