import QtQuick 1.0

MouseArea{
    property string direction: ""
    anchors.fill: parent
    onClicked: manager.movePlaylistEntry(direction, index)
}
