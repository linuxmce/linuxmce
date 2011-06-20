import QtQuick 1.0

Rectangle {
    property alias rowlabel: rowheader.buttontext
    radius: 1
    border.color: "#000000"
    height: 80
    width: parent.width
    color: "orange"
    Row {
        id: telecomRow
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
            buttontext: "Telecom"

            }

        ButtonSq {
            id: activeCalls
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Active Calls"

            }

        ButtonSq {
            id:speedDial
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Speed Dial"
            }

        ButtonSq {
            id:directDial
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Direct Dial"
            }

        ButtonSq {
            id:phoneBook
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Phone Book"
            }

        ButtonSq {
            id:intercom
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Intercom"
            }

        ButtonSq {
            id:floorplan
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Mailbox"
            }

    }
}
