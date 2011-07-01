import QtQuick 1.0




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
            model: lightingmodel
            orientation:ListView.Horizontal
            delegate: ButtonSq {
                id: but
                height: style.buttonH
                width: style.buttonW
                color: style.button_action_color
                radius: style.but_smooth
                buttontext: name
                buttontextfontsize: 12
                buttontextbold: true
                buttontextzindex: 30


                Image {
                    source: img
                    height: style.iconHeight
                    width: style.iconWidth
                    anchors.centerIn: parent
                        }
                    }
                }
            }
        }
}
