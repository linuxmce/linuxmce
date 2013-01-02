import QtQuick 1.0
/*!
 *\class AlarmToggleHandler
 *\brief Handler for toggling the state of sleeping alarms
 *
 *\ingroup lib_handlers
 *
 * The predefined component will toggle the connected alarm state when triggered
 *

 *
 */
MouseArea{
    anchors.fill: parent
    onClicked: {
        manager.updateAlarm(!status, handler)
    }
}
