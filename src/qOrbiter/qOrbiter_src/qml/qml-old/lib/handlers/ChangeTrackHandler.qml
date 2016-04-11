import QtQuick 1.0
/*!
 *\class ChangeTrackHandler
 *\brief Handler Component for Changing track.
 *
 *\ingroup lib_handlers
 *\todo add validation for input
 */

MouseArea{
    property string trackDirection /*! takes value of +1 or -1  */
    anchors.fill: parent                
    onClicked: manager.newTrack(trackDirection)
}
