import QtQuick 1.0

Rectangle {
    property alias bottomPanelTextColor:adv_but_text.color


    width: 640
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
        anchors.centerIn: parent
        height: 50
        width: 50

        MouseArea{
            anchors.fill: parent
            onClicked: close()
        }

    }
}
