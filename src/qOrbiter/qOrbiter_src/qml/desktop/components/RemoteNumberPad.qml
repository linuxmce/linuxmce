import QtQuick 1.0

Rectangle {
    width: 300
    height: 420
    color:"transparent"
    id:remotenumberpad

    Rectangle {
        id: rectangle1
        x: 21
        y: 17
        width: 266
        height: 411
        color: "#789199"
        radius: 22
        anchors.centerIn: parent

        Rectangle {
            id: rectangle2
            x: 10
            y: 13
            width: 231
            height: 43
            color: "#ffffff"
            radius: 19

            TextInput {
                id: text_input1
                x: 0
                y: 0
                text: ""

                font.family: "Droid Sans Mono"
                cursorVisible: true
                readOnly: false
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                horizontalAlignment: TextInput.AlignHCenter
                anchors.fill: parent
                font.pixelSize: 23
            }
        }

        Rectangle {
            id: rectangle3
            x: 10
            y: 103
            width: flow1.width +5
            height: flow1.height +5
            color: "transparent"
            radius: 28
            anchors.centerIn: parent

            Flow {
                id: flow1
                width: 245
                height: 245

                anchors.centerIn: parent
                spacing: 5
                Repeater{
                    model: 10;

                    Rectangle{
                        height: 75
                        width: 77
                        border.color: "black"
                        border.width: 2

                        Text {
                            id: buttonumber
                            text: index
                            font.pointSize: 36
                            font.bold: true
                            anchors.centerIn: parent
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: text_input1.text = text_input1.text+index
                        }

                    }

                }
            }
            ButtonSq{
                id:backbutton
                x: 197
                y: 263
                height:75
                width: 75
                anchors.rightMargin: 10
                anchors.bottomMargin: -68
                buttontext: "Back"
                buttontextbold: true
                buttontextfontsize: 12
                anchors.bottom: flow1.bottom
                anchors.right: flow1.right

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        text_input1.text.trim(1)
                    }
                }
            }
            ButtonSq{
                x: 197
                y: 263
                height: 75
                width: 75
                anchors.rightMargin: 5
                anchors.bottomMargin: -68
                buttontext: "go"
                buttontextbold: true
                buttontextfontsize: 12
                anchors.bottom: flow1.bottom
                anchors.right: backbutton.left
                MouseArea{
                    anchors.fill: parent
                    onClicked:{
                        changeChannels(text_input1.text)
                        text_input1.text=""
                    }
                }

            }
        }
    }

}


