import QtQuick 2.0
Item{
    width: scaleX(20)
    height: remotenumberpad.height
    property alias pass: text_input1.text

    BorderImage {
        id: borderimg
        horizontalTileMode: BorderImage.Repeat
        source: "../img/icons/drpshadow.png"
        anchors.fill: remotenumberpad
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }

    Rectangle {
        width: scaleX(22)
        height: childrenRect.height + scaleY(2)
        color:style.lighthighlight
        id:remotenumberpad
        border.color: style.highlight1
        border.width: 2
        radius:2.5
        anchors.centerIn: parent

        Column{
            id:padcolumn
            spacing:scaleY(2)
            width:parent.width
            height: childrenRect.height + scaleY(2)
            anchors.centerIn: parent

            Rectangle {
                id: numberdisplay
                width: numberrect.width
                height: scaleY(8)
                color: style.highlight1
                radius: 5
                anchors.horizontalCenter: parent.horizontalCenter
                border.color: style.accentcolor
                border.width: 2

                StyledTextInput {
                    id: text_input1
                    text: ""
                    font.family: "Droid Sans Mono"
                    cursorVisible: true
                    readOnly: false
                    anchors.centerIn: numberdisplay
                    horizontalAlignment: StyledTextInput.AlignHCenter
                    anchors.fill: parent
                    font.pixelSize: 23
                }
            }

            Rectangle{
                id:numberrect
                height: childrenRect.height + scaleY(1)
                width: childrenRect.width + scaleX(1)
                anchors.top: numberdisplay.bottom
                anchors.topMargin: 5
                anchors.horizontalCenter: parent.horizontalCenter
                border.color: style.highlight2
                color: style.highlight1
                radius: 2.5
                Column{
                    id: numberrowcolum
                    spacing: scaleY(1)
                    anchors.centerIn: parent
                    Row{
                        id:toprow
                        spacing: scaleX(1)
                        //number one
                        AvOptionButton{
                            id:number1
                            buttontext: qsTr("1")
                             buttontextfontsize: 12
                            MouseArea
                            {
                                anchors.fill: parent

                                onClicked:
                                {
                                    text_input1.text = text_input1.text + "1"
                                }
                            }
                        }
                        //number two
                        AvOptionButton{
                            id:number2
                            buttontext: qsTr("2")
                             buttontextfontsize: 12
                            MouseArea
                            {
                                anchors.fill: parent

                                onClicked:
                                {
                                    text_input1.text = text_input1.text + "2"
                                }

                            }
                        }
                        //number 3
                        AvOptionButton{
                            id:number3
                            buttontext: qsTr("3")
                             buttontextfontsize: 12
                            MouseArea
                            {
                                anchors.fill: parent

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
                        AvOptionButton{
                            id:number4
                            buttontext: qsTr("4")
                             buttontextfontsize: 12
                            MouseArea
                            {
                                anchors.fill: parent

                                onClicked:
                                {
                                    text_input1.text = text_input1.text + "4"
                                }
                            }
                        }
                        //number 5
                        AvOptionButton{
                            id:number5
                            buttontext: qsTr("5")
                             buttontextfontsize: 12
                            MouseArea
                            {
                                anchors.fill: parent

                                onClicked:
                                {
                                    text_input1.text = text_input1.text + "5"
                                }
                            }
                        }
                        //number 6
                        AvOptionButton{
                            id:number6
                            buttontext: qsTr("6")
                             buttontextfontsize: 12
                            MouseArea
                            {
                                anchors.fill: parent

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
                        AvOptionButton{
                            id:number7
                            buttontext: qsTr("7")
                             buttontextfontsize: 12
                            MouseArea
                            {
                                anchors.fill: parent

                                onClicked:
                                {
                                    text_input1.text = text_input1.text + "7"
                                }
                            }
                        }
                        //number 8
                        AvOptionButton{
                            id:number8
                            buttontext: qsTr("8")
                            color: "black"
                             buttontextfontsize: 12
                            MouseArea
                            {
                                anchors.fill: parent

                                onClicked:
                                {
                                    text_input1.text = text_input1.text + "8"
                                }

                            }
                        }
                        //number 9
                        AvOptionButton{
                            id:number9
                            buttontext: qsTr("9")
                             buttontextfontsize: 12
                            color: "black"
                            MouseArea
                            {
                                anchors.fill: parent

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

                        AvOptionButton{
                            id:backbutton
                            buttontext: "Back"
                            buttontextbold: true
                            buttontextfontsize: 12
                            buttonsqradius:2.5
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    text_input1.text = ""
                                }
                            }
                        }

                        //number 9
                        AvOptionButton{
                            id:number0
                            buttontext: qsTr("0")
                            color: "black"

                            MouseArea
                            {
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: text_input1.text = text_input1.text + "9"

                            }
                        }
                    }
                }
            }
        }
    }
}
