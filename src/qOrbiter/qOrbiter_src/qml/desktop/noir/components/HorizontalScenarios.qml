import QtQuick 1.1
Rectangle{
    color: "transparent"
    height: buttonHeight
    width: childrenRect.width

    z:30
    visible: popEnabled ? true : false
    ListView{
        id:scenarioList
        height: 0
        z:2
        width:manager.appWidth*80
        model: scenarioModel
        orientation:ListView.Horizontal

        spacing:10
        delegate: Rectangle{
            height: 50
            width: componentWidth
            color: delegateHit.containsMouse ? delegateHit.pressed ? style.darkhighlight : style.darkhighlight : style.highlight1
           // color: "transparent"
            StyledText {
                id: scenarioItem
                text: title
                font.pixelSize: 12
                anchors.centerIn: parent
                color: "White"
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
}
