import QtQuick 1.1
/*!
 *\class MediaListClickHandler
 *\brief Handler Component for media listmodel.
 *
 *\ingroup lib_handlers
 *\todo list params
 */
MouseArea{
    signal activated()
    //only for use as a click handler with the media list. It takes the id of the clicked item and decides if its a file or folder
    anchors.fill: parent
    onReleased: {manager.addRestoreIndex(model.index); manager.setStringParam(4, id); activated()}
}
