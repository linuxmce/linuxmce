import QtQuick 1.0

Rectangle {
    width: scaleX(30)
    height: childrenRect.height
    color:style.accentcolor
    id:remotenumberpad

    Column{
        id:padcolumn
        spacing:5
        width:parent.width
        height: childrenRect.height
        anchors.centerIn: parent

        Rectangle {
            id: numberdisplay
            width: parent.width
            height: scaleY(8)
            color: "#ffffff"
            radius: 19

            TextInput {
                id: text_input1
                text: ""
                font.family: "Droid Sans Mono"
                cursorVisible: true
                readOnly: false
                anchors.centerIn: numberdisplaybox
                horizontalAlignment: TextInput.AlignHCenter
                anchors.fill: parent
                font.pixelSize: 23
            }
        }
        Rectangle{
            id:flowrect
            height: scaleY(57)
            width: parent.width
            color: style.darkhighlight

            Flow {
                id: flow1
                width:parent.width
                height: flowrect.height
                anchors.centerIn: flowrect
                spacing: 5
                Repeater{
                    model: 10;
                    Rectangle{
                        height: style.stdbuttonh
                        width: style.stdbuttonw
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
        }

        Row{
            anchors.bottom: flowrect.bottom
            anchors.right: padcolumn.right
            height: childrenRect.height
            width: childrenRect.width
            ButtonSq{
                id:backbutton
                height:style.stdbuttonh
                width: style.stdbuttonw
                buttontext: "Back"
                buttontextbold: true
                buttontextfontsize: 12
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        text_input1.text.trim(1)
                    }
                }
            }
            ButtonSq{
                height:style.stdbuttonh
                width: style.stdbuttonw
                buttontext: "go"
                buttontextbold: true
                buttontextfontsize: 12
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



