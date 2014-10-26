import QtQuick 1.1
/*!
 *\class PlaylistClickedHandler
 *\brief Handler Component for Playlist click / enter.
 *Place this handler in the delegate of a playlist and it will change the track accordingly.

 *\ingroup lib_handlers
 *\warning 'Enter' key not implemented.
 *\note use: PlaylistClickedHandler{}
 */
MouseArea{
    anchors.fill: parent
    onClicked: {manager.changedPlaylistPosition(index); console.log("Change track")}
}
