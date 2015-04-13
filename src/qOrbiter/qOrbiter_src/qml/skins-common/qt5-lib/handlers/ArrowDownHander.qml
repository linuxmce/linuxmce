import QtQuick 2.2
/*!
 *\class ArrowDownHander
 *\brief Handler Component for down Arrow.
 *
 *\ingroup lib_handlers
 *The down arrow handler has no parameters. It simply does the job of emitting the move down signal to Linuxmce
 *\note Use:ArrowDownHandler{}
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
