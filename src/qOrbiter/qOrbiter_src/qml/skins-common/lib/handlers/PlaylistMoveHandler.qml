import QtQuick 1.1
/*!
 *\class PlaylistMoveHandler
 *\brief Handler Component for moving playlist item.
 *
 *\ingroup lib_handlers
 *This moves a playlist item in the using the direction property
 */
MouseArea{
    property string direction: "" /*! Values accepted are + or - */
    property string translatedDirection:direction==="+" ? "+" : ""
    anchors.fill: parent
    onClicked: manager.movePlaylistEntry(translatedDirection, index)
}
