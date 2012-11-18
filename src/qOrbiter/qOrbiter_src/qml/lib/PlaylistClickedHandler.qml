import QtQuick 1.0

MouseArea{
    anchors.fill: parent
    onClicked: manager.changedPlaylistPosition(index)
}
