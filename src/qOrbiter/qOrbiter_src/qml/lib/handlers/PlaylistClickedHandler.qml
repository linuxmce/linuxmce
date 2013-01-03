import QtQuick 1.0
/*!
 *\class PlaylistClickedHandler
 *\brief Handler Component for Playlist click / enter.
 *
 *\ingroup lib_handlers
 *\warning 'Enter' key not implemented.
 */
MouseArea{
    anchors.fill: parent
    onClicked: manager.changedPlaylistPosition(index)
}
