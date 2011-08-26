import QtQuick 1.0
Rectangle {
    id:buttonBase

    property alias textSize: buttonLabel.font.pointSize
    signal clicked
    property variant text

    height: buttonLabel.height + 4;
    width: buttonLabel.width + 8
        color: style.button_system_color
        border.width: 1
        border.color: style.button_action_color
        radius: 1
        smooth:  true
        opacity: 1

    Text {
        id: buttonLabel
        text:parent.text
        font.pointSize: 10
        anchors.centerIn:parent
        font.family: "Droid Sans"
        wrapMode: Text.WordWrap

         }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: buttonBase.color=style.button_system_color_hover
        onExited:  buttonBase.color=style.button_system_color
        onClicked: buttonBase.clicked()
    }

}
