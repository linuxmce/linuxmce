import QtQuick 1.0

MouseArea{
    property string direction: ""
    property string translatedDirection:direction==="+" ? "+" : ""
    anchors.fill: parent
    onClicked: manager.movePlaylistEntry(translatedDirection, index)
}
