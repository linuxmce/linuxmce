import QtQuick 1.0
import "."
/*!
 *\class AlarmToggleHandler
 *\brief Handler for toggling the state of sleeping alarms.
 *
 *\ingroup lib_handlers
 *
 * The predefined component will toggle the connected alarm state when triggered
 *\note no parameters
 *\note Use: AlarmToggleHandler{}
 */
MouseArea{
    anchors.fill: parent
    onClicked: {
        manager.updateAlarm(!status, handler)
    }
}
