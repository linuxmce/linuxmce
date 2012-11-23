import QtQuick 1.0


MouseArea{
    anchors.fill: remove_box
    onClicked: manager.removePlaylistItem(index)
}
