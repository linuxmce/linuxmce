import QtQuick 1.0

MouseArea {
    property int autoTick:0
    anchors.fill: parent
    hoverEnabled: true
    Timer{
        id:tickTimer
        running:autoTick === 0 ? false : true
        interval: autoTick
        repeat: true
        onTriggered: manager.moveDirection(3)
    }
}
