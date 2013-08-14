import QtQuick 1.1

Item{
    id:btn
    height: 65
    width: childrenRect.width +10
    signal activated()
    property string label: ""
    Rectangle{
        id:filler
        color:ms.pressed ? system.blueHighlight : "white"
        anchors.fill: parent
    }
    
    Text{
        anchors.centerIn: parent
        text:btn.label
        font.pixelSize: 22
        font.weight: Font.Light
        color:"black"
    }
    
    MouseArea{
        id:ms
        anchors.fill: parent
        onReleased: activated()
    }
}
