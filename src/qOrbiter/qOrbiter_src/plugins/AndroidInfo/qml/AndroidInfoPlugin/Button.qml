import QtQuick 1.1

Item{
    id:btn
    height: 65
    width: txt.width +10
    signal activated()
    property string label: ""
    property string labelColor:system.greenHighlight
    Rectangle{
        id:filler
        color:ms.pressed ? "white": labelColor
        anchors.fill: parent
        border.width:2
        border.color:"white"
        radius:2

        Behavior on color{
            PropertyAnimation{
                duration:550

            }
        }
    }
    
    Text{
        id:txt
        anchors.centerIn: parent
        text:btn.label
        font.pixelSize: 24
        font.weight: Font.Light
        font.capitalization: Font.SmallCaps
        color:"white"
    }
    
    MouseArea{
        id:ms
        anchors.fill: parent
        onReleased: activated()
    }
}
