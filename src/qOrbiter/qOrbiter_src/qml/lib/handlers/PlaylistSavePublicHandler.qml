import QtQuick 1.0

MouseArea{
    //set the property playlist name to your text areas' id.
    property string playlistname: ""
    anchors.fill: parent
    onClicked: manager.saveCurrentPlaylist(playlistname.text, true)
}
