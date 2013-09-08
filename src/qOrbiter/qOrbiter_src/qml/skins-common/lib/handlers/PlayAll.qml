import QtQuick 1.1
/*!
 *\class PlayAll
 *\brief Handler Component for Play All button.
 *
 *\ingroup lib_handlers
 *
 */
MouseArea{
    anchors.fill:parent
    onClicked: manager.playMedia("!G"+iPK_Device)
}
