import QtQuick 1.1

ListView{
    id:scenarioList
    height: 0
    width: componentWidth
    model: scenarioModel

    // Component.onCompleted:

   delegate: Rectangle{
        height: 50
        width: componentWidth
        color: delegateHit.containsMouse ? delegateHit.pressed ? delegatePressedColor : delegateHoverColor : delegateActiveColor
        Text {
            id: scenarioItem
            text: title
            font.pixelSize: 12
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            id:delegateHit
            hoverEnabled: true
            onClicked:manager.execGrp(params);
            onExited: closeTimer.start()
            onEntered: closeTimer.stop()
        }
    }
    
    
}
