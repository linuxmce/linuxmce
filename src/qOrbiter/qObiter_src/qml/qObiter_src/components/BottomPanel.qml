import QtQuick 1.0

Rectangle {
    property alias bottomPanelTextColor:adv_but_text.color


    width: style.orbiterW
    height: 50
    id:advanced_panel
            Text {
            id: adv_but_text
            text: "Advanced"
            font.pointSize: 14
            anchors.left: parent.left
                 }

    ButtonSq{
        id:exit
        buttontext: "Exit Orbiter"
        buttontextcolor: style.button_system_color
        anchors.left: user.right
        anchors.leftMargin: 15
        height: 50
        width: 50

        MouseArea{
            anchors.fill: parent
            onClicked: close()
                }
             }

        Rectangle{
            id:user
            anchors.centerIn: parent
            height: 50
            width: 50
            color: "slategrey"

            Image {
                id: bg
                source: "../../../img/icons/personal.png"
                opacity: 5
                height:50
                width: 50
                anchors.centerIn: parent

            }
            Text {
                id: user_text
                text: currentuser
                font.bold: true
                font.pointSize: 12
                color:"White"
                anchors.centerIn: parent

            }

        }



}
