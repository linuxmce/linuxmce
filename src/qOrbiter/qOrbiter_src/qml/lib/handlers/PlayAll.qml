import QtQuick 1.1

MouseArea{
    anchors.fill:parent
    onClicked: manager.playMedia("!G"+iPK_Device)
}
