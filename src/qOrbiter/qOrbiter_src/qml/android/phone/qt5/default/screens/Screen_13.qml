import QtQuick 2.2
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
        height: 430
        width: 800
        color: style.bgcolor
        Text {
            id: securitypanellabel
            x: 268
            y: 0
            text: "Security Panel"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }


        HomeButton{ x: 321; y: 344; width: 75; height: 75; smooth: true}

        Rectangle {
            id: rectangle1
            x: 21
            y: 17
            width: 250
            height: 400
            color: "#789199"
            radius: 22

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
                    echoMode: TextInput.Password
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
                width: 231
                height: 245
                color: "#ffffff"
                radius: 28

                Flow {
                    id: flow1
                    anchors.fill: parent
                    spacing: 5
                    Repeater{
                        model: 10;

                        Rectangle{
                            height: 25
                            width: 25
                            anchors.margins: 10
                            Text {
                                id: buttonumber
                                text: index
                                font.pointSize: 24
                                font.bold: true
                                }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: text_input1.text = text_input1.text+index
                            }
                           }
                    }
                }
            }
        }

        Rectangle {
            id: rectangle4
            x: 584
            y: 17
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


