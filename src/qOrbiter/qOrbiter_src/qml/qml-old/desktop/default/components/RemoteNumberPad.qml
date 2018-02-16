import QtQuick 1.1

Rectangle {
    width: scaleX(20)
    height: childrenRect.height
    color:skinstyle.lighthighlight
    id:remotenumberpad
    border.color: skinstyle.highlight1
    border.width: 2
    radius:5


    Column{
        id:padcolumn
        spacing:5
        width:parent.width
        height: childrenRect.height + scaleY(1)
        // anchors.centerIn: parent

        Rectangle {
            id: numberdisplay
            width: scaleX(15)
            height: scaleY(8)
            color: skinstyle.highlight2
            radius: 19
            anchors.horizontalCenter: parent.horizontalCenter
            border.color: skinstyle.accentcolor
            border.width: 2

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
            height: childrenRect.height + scaleY(1)
            width: childrenRect.width
            // anchors.top: numberdisplay.bottom
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            border.color: skinstyle.highlight1
            color: skinstyle.darkhighlight
            radius: 5
            Column{
                id: numberrowcolum
                spacing: scaleY(1)

                Row{
                    id:toprow
                    spacing: scaleX(1)
                    //number one
                    Rectangle{
                        id:number1
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {
                            id: name
                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                    spacing: scaleX(1)

                    //number 4
                    Rectangle{
                        id:number4
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                    spacing: scaleX(1)
                    //number 7
                    Rectangle{
                        id:number7
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
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
                    spacing: scaleX(1)
                    ButtonSq{
                        id:backbutton
                        height: scaleY(9)
                        width: scaleX(5)
                        buttontext: "Back"
                        buttontextbold: true
                        buttontextfontsize: 12
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
                        height: scaleY(9)
                        width: scaleX(5)
                        color:skinstyle.lighthighlight
                        radius: 100
                        Image {

                            source: "../img/icons/rndbuttonbase.png"
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
                                parent.color = skinstyle.darkhighlight
                            }
                            onExited:
                            {
                              parent.color = skinstyle.lighthighlight
                            }
                             onClicked: text_input1.text = text_input1.text+"0"

                        }
                    }




                    AvOptionButton{
                         height: scaleY(9)
                       width: scaleX(5)
                        buttontext: "go"

                        MouseArea{
                            anchors.fill: parent
                            onClicked:{
                                manager.gridChangeChannel(text_input1.text, text_input1.text)
                                text_input1.text=""
                            }
                        }
                    }
                }
            }
        }
    }
}



