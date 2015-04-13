import QtQuick 2.2

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
            height: style.stdbuttonh
            width: style.stdbuttonw
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: rowHD

            }
    }
}
