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
                anchors.centerIn: numberdisplay
                horizontalAlignment: TextInput.AlignHCenter
                anchors.fill: parent
                font.pixelSize: 23
            }
        }

        Rectangle{
            id:numberrect
            height: scaleY(57)
            width: childrenRect.width
            anchors.top: numberdisplay.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            border.color: style.darkhiglight
            color: style.lighthighlight
            Column{



                Row{
                    id:toprow
                     spacing: scaleX(2)
                    //number one
                    Rectangle{
                        id:number1
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: numberonelabel
                            text: qsTr("1")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number1.scale = 1.15

                            }
                            onExited:
                            {
                                number1.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "1"
                            }

                        }
                    }
                    //number two
                    Rectangle{
                        id:number2
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: numbero2label
                            text: qsTr("2")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number2.scale = 1.15

                            }
                            onExited:
                            {
                                number2.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "2"
                            }

                        }
                    }
                    //number 3
                    Rectangle{
                        id:number3
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: number3label
                            text: qsTr("3")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number3.scale = 1.15

                            }
                            onExited:
                            {
                                number3.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "3"
                            }

                        }
                    }

                }

                //secondrow
                Row
                {
                    id:middlerow
                    spacing: scaleX(2)

                    //number 4
                    Rectangle{
                        id:number4
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: numberonelabe4
                            text: qsTr("4")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number4.scale = 1.15

                            }
                            onExited:
                            {
                                number4.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "4"
                            }

                        }
                    }
                    //number 5
                    Rectangle{
                        id:number5
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: numbero5label
                            text: qsTr("5")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number5.scale = 1.15

                            }
                            onExited:
                            {
                                number5.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "5"
                            }

                        }
                    }
                    //number 6
                    Rectangle{
                        id:number6
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: number6label
                            text: qsTr("6")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number6.scale = 1.15

                            }
                            onExited:
                            {
                                number6.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "6"
                            }

                        }
                    }

                }

                //bottom row
                Row
                {
                    id:bottomrow
                     spacing: scaleX(2)
                    //number 7
                    Rectangle{
                        id:number7
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: number7label
                            text: qsTr("7")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number7.scale = 1.15

                            }
                            onExited:
                            {
                                number7.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "7"
                            }

                        }
                    }
                    //number 8
                    Rectangle{
                        id:number8
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: number8label
                            text: qsTr("8")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number8.scale = 1.15

                            }
                            onExited:
                            {
                                number8.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "8"
                            }

                        }
                    }
                    //number 9
                    Rectangle{
                        id:number9
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: number9label
                            text: qsTr("9")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number9.scale = 1.15

                            }
                            onExited:
                            {
                                number9.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "9"
                            }

                        }
                    }
                }

                Row{
                    id:actionbuttons
                    height: childrenRect.height
                    width: childrenRect.width
                     spacing: scaleX(2)
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
                                text_input1.text = ""
                            }
                        }
                    }

                    //number 9
                    Rectangle{
                        id:number0
                        height: style.stdbuttonh
                        width: style.stdbuttonw
                        color:style.lightdarkhiglight
                        radius: 100
                        Text {
                            id: number0label
                            text: qsTr("0")
                            color: "black"
                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {

                                number0.scale = 1.15

                            }
                            onExited:
                            {
                                number0.scale = 1
                            }

                            onClicked:
                            {
                                text_input1.text = text_input1.text + "0"
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
    }
}



