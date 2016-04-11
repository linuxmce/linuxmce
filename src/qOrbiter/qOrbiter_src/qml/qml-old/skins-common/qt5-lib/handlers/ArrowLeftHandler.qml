import QtQuick 2.2
/*!
 *\class ArrowLeftHandler
 *\brief Handler Component for Left Arrow.
 *This Component simply emits the arrow left signal. It has two modes
 -Clicked will simply emit the signal once
 -Press and Hold will result in a repeat, with the autoTick property set to 400
 *\ingroup lib_handlers
 *\note Use: ArrowLeftHandler{}
 */
MouseArea {
    id:arrowLeftArea
    property int autoTick:400
    anchors.fill: parent
    hoverEnabled: true
    onClicked: manager.moveDirection(3)
    onPressAndHold: tickTimer.start()
    onReleased: tickTimer.stop()
    Timer{
        id:tickTimer
        running:autoTick === 0 ? false : true
        interval: autoTick
        repeat: arrowLeftArea.pressed
        onTriggered: manager.moveDirection(3)
    }
}
