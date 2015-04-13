import QtQuick 2.2
/*!
 *\class MediaListClickHandler
 *\brief Handler Component for media listmodel.
 *
 *\ingroup lib_handlers
 *\todo list params
 */
MouseArea
{
    //only for use as a click handler with the media list. It takes the id of the clicked item and decides if its a file or folder
    anchors.fill: parent
    onClicked: {manager.setStringParam(4, id);}
}
