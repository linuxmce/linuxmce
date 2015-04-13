import QtQuick 2.2
/*!
 *\class PauseButtonHandler
 *\brief Handler Component for up Arrow.
 *
 *\ingroup lib_handlers
 *
 */

MouseArea {
    anchors.fill: parent                
    onClicked: manager.pauseMedia()
}
