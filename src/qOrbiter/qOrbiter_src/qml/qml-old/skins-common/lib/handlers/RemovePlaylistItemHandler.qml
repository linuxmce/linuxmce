import QtQuick 1.1
/*!
 *\class RemovePlaylistItemHandler
 *\brief Handler Component for Removing playlistItem.
 *
 *\ingroup lib_handlers
 *This removes a playlist item
 *\todo delete is duplicate.
 */
MouseArea{
    anchors.fill: parent
    onClicked: manager.movePlaylistEntry("", index)
}
