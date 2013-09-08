import QtQuick 1.1
/*!
 *\class ArrowRightHandler
 *\brief Handler Component for right Arrow.
 *This Component simply emits the arrow up signal. It has two modes
 -Clicked will simply emit the signal once
 -Press and Hold will result in a repeat, with the autoTick property set to 400
 *\ingroup lib_handlers
 *\note Use: ArrowRightHandler{}
 */
MouseArea {
    id:arrowUpArea
    property int autoTick:400
    anchors.fill: parent
    hoverEnabled: true
    onClicked: manager.moveDirection(4)
    onPressAndHold: tickTimer.start()
    onReleased: tickTimer.stop()

    Timer{
        id:tickTimer
        running:autoTick === 0 ? false : true
        interval: autoTick
        repeat: arrowUpArea.pressed
        onTriggered: manager.moveDirection(4)
    }
}
