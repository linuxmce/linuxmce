import QtQuick 1.1
/*!
 *\class ArrowUpHandler
 *\brief Handler Component for up Arrow.
 *This Component simply emits the arrow down signal. It has two modes
 -Clicked will simply emit the signal once
 -Press and Hold will result in a repeat, with the autoTick property set to 400
 *\ingroup lib_handlers
 */
MouseArea {
    id:arrowDownArea
    property int autoTick:400
    anchors.fill: parent
    hoverEnabled: true
    onClicked: manager.moveDirection(2)
    onPressAndHold: tickTimer.start()
    onReleased: tickTimer.stop()

    Timer{
        id:tickTimer
        running:autoTick === 0 ? false : true
        interval: autoTick
        repeat: arrowDownArea.pressed
        onTriggered: manager.moveDirection(2)
    }
}
