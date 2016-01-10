import QtQuick 2.2
/*!
 *\class StopMediaHandler
 *\brief Handler Component for Stopping media.
 *
 *\ingroup lib_handlers
 *Stops media in the current EA when clicked. No params
 *params none
 */
MouseArea
{
    anchors.fill: parent                 
    onClicked: manager.stopMedia()
}
