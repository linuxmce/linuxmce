import Qt 4.7
Rectangle {
    property alias rowlabel: rowheader.buttontext
    height: 65
    width: parent.width
    color:style.rowbgColor

    ListModel{
        id:lightingmodel


        ListElement{
            name:"On"
            device: "qml light switch"
            msgsnd: "/usr/pluto/bin/MessageSend localhost 0 34 1 192 97 "
            img: "../../../img/icons/ktip.png"

        }

        ListElement{
            name:"Off"
            device: "qml light switch"
            msgsnd: "/usr/pluto/bin/MessageSend localhost 0 34 1 192 97 "
            img: "../../../img/icons/jabber_protocol.png"
            }
        }


    Flickable{
        interactive: true
        height: 65
        width: parent.width
        contentHeight: 65
        contentWidth: childrenRect * 1
        clip: true

Row {
        id: guide
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        spacing:5

        ButtonSq {
            id: rowheader
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""
            Image {
                id: onimg
                source: "../../../img/icons/jabber_protocol.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent
                    }
              }

        ListView{
            id: lightingScenarios
            width: 300
            height: 50
            model: currentRoomLights
            orientation:ListView.Horizontal
            delegate: ButtonSq {

                id: but
                height: style.buttonH
                width: style.buttonW
                color: style.button_action_color
                radius: style.but_smooth
                buttontext: label
                buttontextfontsize: 12
                buttontextbold: true
                buttontextzindex: 30



                MouseArea{
                    anchors.fill: parent
                    onClicked: console.log ("I am button:" + index + ". My Name is:"+ label+". My Params:" + params +". And my Command:" + command)
                }


                Image {
                    source: "../../../img/icons/"+label+".png"
                    height: style.iconHeight
                    width: style.iconWidth
                    anchors.centerIn: parent
                        }
                ListView.onRemove: SequentialAnimation {
                            PropertyAction { target: lightingScenarios; property: "ListView.delayRemove"; value: true }
                            NumberAnimation { target: lightingScenarios; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                            PropertyAction { target: lightingScenarios; property: "ListView.delayRemove"; value: false }
                        }
                    }

                    }
                }
            }
        }

