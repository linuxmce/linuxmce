import QtQuick 1.0

MouseArea{
    anchors.fill: parent
    hoverEnabled: true

    onClicked: manager.moveDirection(2)
}
