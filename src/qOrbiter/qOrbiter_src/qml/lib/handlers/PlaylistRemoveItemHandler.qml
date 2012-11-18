import QtQuick 1.0
import "../lib"

MouseArea{
    anchors.fill: remove_box
    onClicked: manager.removePlaylistItem(index)
}
