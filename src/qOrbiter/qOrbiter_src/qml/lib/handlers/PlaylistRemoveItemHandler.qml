import QtQuick 1.0
/*!
 *\class PlaylistRemoveItemHandler
 *\brief Handler Component for removing a playlist item.
 *
 *\ingroup lib_handlers
 *This removes an item from a playlist and has no properties.
 *it takes the index of the item clicked and asks the router to remove that item.
 */
MouseArea{
    anchors.fill: parent
    onClicked: manager.removePlaylistItem(index)
}
