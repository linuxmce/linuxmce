import QtQuick 1.0
import "../components"
Item{
    id:securitypanel

    ListModel{
        id:securityModes

        ListElement{
            name: "Home"
            image: "security/Home.png"
        }

        ListElement{
            name: "Away"
            image: "security/Away.png"
        }

        ListElement{
            name: "At Home Secure"
            image: "security/HomeSecure.png"
        }

        ListElement{
            name: "Away"
            image: "security/Away.png"
        }

        ListElement{
            name: "Entertaining"
            image: "security/Entertaining.png"
        }

        ListElement{
            name: "Sleeping"
            image: "security/Sleeping.png"
        }

        ListElement{
            name: "Extended Away"
            image: "security/Extended.png"
        }

    }

    //delegate
    Component {
        id: securityDelegate
        Item{
            height: childrenRect.height
            width: childrenRect.width
            clip: true

            Rectangle{
                id:secrect
                height:75
                width: securitylist.width
                color: "whitesmoke"
                opacity: .5
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        setHouseMode(index+1, text_input1.text)
                    }
                }

            }

            Text {
                id: delegatetext
                text: name
                anchors.centerIn: secrect
                font.pointSize: 12
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
            }
        }
    }

    Rectangle{
        id:container
        height: style.orbiterH
        width: style.orbiterW
        color: style.darkhighlight

        Text {
            id: securitypanellabel
            anchors.top: stage.top
            text: "Security Panel"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }

        Rectangle{
            height:childrenRect.height
            width: childrenRect.width
            border.color: style.accentcolor
            border.width: 2
            anchors.centerIn: parent
            color: style.lighthighlight

            Row{
                height: childrenRect.height
                width: childrenRect.width
                spacing: scaleX(5)
                anchors.centerIn: parent
                HomeButton{height: 75; smooth: true}

                Column{
                    id:padcolumn
                    spacing:5
                    width:(style.stdbuttonh *3) +15
                    height: childrenRect.height+10


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
                            echoMode: TextInput.passwordCharacter
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
                                    height: scaleY(7)
                                    width: scaleX(7)
                                    border.color: "black"
                                    border.width: 2
                                    Text {
                                        id: buttonumber
                                        text: (index+1 > 9) ? 0 : index+1
                                        font.pointSize: 25
                                        font.bold: true
                                        anchors.centerIn: parent
                                    }
                                    MouseArea{
                                        anchors.fill: parent
                                        onClicked: text_input1.text = text_input1.text+ ((index+1 > 9) ? 0 : index+1)
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    id: rectangle4
                    width: 200
                    height: 402
                    color: "#a7b8c4"
                    radius: 16
                    clip: true

                    ListView
                    {
                        id:securitylist
                        height: parent.height
                        width: parent.width
                        clip: true
                        model: securityModes
                        orientation: ListView.Vertical
                        delegate: securityDelegate

                    }
                }
            }
        }
    }
}

