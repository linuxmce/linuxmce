import QtQuick 2.2

Rectangle {
    width: padcolumn.width
    height: numberdisplay.height + remotenumberpad.height
    color:"transparent"
    id:remotenumberpad
    border.color: "orange"
    border.width: 2


    Column{
        id:padcolumn
        spacing:5
        width:scaleX(45)
        height: childrenRect.height + scaleY(1)
        anchors.top: remotenumberpad.top
        anchors.horizontalCenter: remotenumberpad.horizontalCenter

        Rectangle {
            id: numberdisplay
            width: numberrect.width
            height: scaleY(5)
            color: style.highlight2
            radius: 2
            anchors.horizontalCenter: parent.horizontalCenter
            border.color: style.accentcolor
            border.width: 1

            TextInput {
                id: text_input1
                text: ""

                font.family: "Droid Sans Mono"
                cursorVisible: true
                readOnly: false
                // anchors.centerIn: numberdisplay
                horizontalAlignment: TextInput.AlignHCenter
                // anchors.fill: parent
                font.pixelSize: 23
            }
        }

        Rectangle{
            id:numberrect
            height: childrenRect.height
            width: childrenRect.width
            // anchors.top: numberdisplay.bottom
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            border.color: style.highlight1
            color: "transparent"
            radius: 5
            Column{
                id: numberrowcolum
                spacing: 5

                Row{
                    id:toprow
                    spacing: 8
                    //number one
                    Rectangle{
                        id:number1
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {
                            id: name
                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
                        Text {
                            id: numberonelabel
                            text: qsTr("1")

                            font.pixelSize: parent.height * .50
                            anchors.centerIn: parent
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                    spacing: 8

                    //number 4
                    Rectangle{
                        id:number4
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                    spacing: 8
                    //number 7
                    Rectangle{
                        id:number7
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }
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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
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
                    spacing: 8
                    ButtonSq{
                        id:backbutton
                        height: scaleX(15)
                        width: scaleX(15)

                        buttontext: "Back"

                        buttontextfontsize: 10
                        buttonsqradius: 10

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
                        height: scaleX(15)
                        width: scaleX(15)
                        color:"silver"

                        Image {

                            source: "../img/buttonbg.png"
                            anchors.fill: parent
                        }


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
                                parent.color = "silver"
                            }
                            onExited:
                            {
                              parent.color = "silver"
                            }
                            onClicked: text_input1.text = text_input1.text + "0"

                        }
                    }

                    ButtonSq{
                        id:go
                        height: scaleX(15)
                        width: scaleX(15)
                        buttontext: "Go"
                        buttontextfontsize: 10
                        buttonsqradius: 10

                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                manager.TuneToChannel(text_input1.text, text_input1.text)                               
                               text_input1.text = ""
                            }
                        }
                    }
                }
            }
        }
    }
}



