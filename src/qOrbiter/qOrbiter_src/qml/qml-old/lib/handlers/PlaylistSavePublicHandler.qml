import QtQuick 1.0
/*!
 *\class SavePublicHandler
 *\brief Handler Component for Saving a playlist.
 *
 *\ingroup lib_handlers
 *\warning Not implemented
 */
MouseArea{
    //set the property playlist name to your text areas' id.
    property string playlistname: ""
    anchors.fill: parent
    onClicked: manager.saveCurrentPlaylist(playlistname.text, true)
}
