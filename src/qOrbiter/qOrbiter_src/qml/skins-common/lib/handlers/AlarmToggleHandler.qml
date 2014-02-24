import QtQuick 1.1
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
    id:alarmMouseArea
    anchors.fill: parent
    signal activated()
    onClicked: {
        alarmlist.model.setData(index, "status", !status)
        activated()
    }
}
