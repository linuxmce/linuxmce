import QtQuick 2.0

Rectangle {
    id:alert_rect
    width: 150
    height: 25

    StyledText {
        id: alert_text
        text: ""
        font.pointSize: 14
        anchors.centerIn: parent
          }

    MouseArea {
        anchors.fill: parent
        onClicked: alert_rect.destroy()
              }
}
