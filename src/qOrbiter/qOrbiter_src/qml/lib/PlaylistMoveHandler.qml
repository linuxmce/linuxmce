import QtQuick 1.0
import "../lib"

MouseArea{
    anchors.fill: parent
    onClicked: manager.movePlaylistEntry("", index)
}
