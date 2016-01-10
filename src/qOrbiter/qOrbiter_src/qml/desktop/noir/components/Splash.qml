import QtQuick 1.1

Rectangle {
    id: rectangle1
    width: 800
    height: 800

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
        source: "../../../../img/lmcesplash.png"
        height: 480
        width: 800
        anchors.centerIn: parent

        Rectangle {
            id: rectangle2
            x: 242
            y: 391
            width: 453
            height: 41
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
            x: 241
            y: 402
            width: 80
            height: 20
            text: "RouterIP"
            font.pixelSize: 12
        }

        TextEdit {
            id: devicenumber
            x: 598
            y: 402
            width: 80
            height: 20
            text: "Device Number"
            font.pixelSize: 12
        }

        ButtonSq {
            id: buttonsq1
            height: 43
            buttonsqradius: 5
            width: 78
            buttontext: "Connect"
            x: 433
            y: 391
        }




    }




}
