import QtQuick 1.0
/*!
 *\class ArrowUpHandler
 *\brief Handler Component for up Arrow.
 *
 *\ingroup lib_handlers
 *
 */
MouseArea {
    property int autoTick:0
    anchors.fill: parent
    hoverEnabled: true
    onClicked: manager.moveDirection(2)
    onPressAndHold: tickTimer.start()
    onReleased: tickTimer.stop()

    Timer{
        id:tickTimer
        running:autoTick === 0 ? false : true
        interval: autoTick
        repeat: true
        onTriggered: manager.moveDirection(2)
    }
}
