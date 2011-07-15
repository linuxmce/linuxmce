import QtQuick 1.0

Rectangle {
    property alias rowlabel: rowheader.buttontext

    height: 80
    width: parent -5

    Row {
        id: guide
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing:5

        ButtonSq {
            id: rowheader
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: rowHD

            }
    }
}
