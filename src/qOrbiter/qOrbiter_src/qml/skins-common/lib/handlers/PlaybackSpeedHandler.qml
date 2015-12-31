import QtQuick 1.1

/*!
 *\class PlaybackSpeedHandler
 *\brief Handler Component for playbackspeed control.
 *
 *\ingroup lib_handlers
 *\todo add click interval restriction
 */
MouseArea{
    property int speed
    anchors.fill: parent                
    onClicked: manager.setPlaybackSpeed(speed)
}
