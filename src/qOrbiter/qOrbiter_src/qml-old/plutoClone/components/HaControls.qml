import QtQuick 2.0
Item{
    height: harect.height
    width: harect.width



BorderImage {
    id: haborder
    horizontalTileMode: BorderImage.Repeat
    source: "../img/icons/drpshadow.png"
    anchors.fill: harect
    anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
    border { left: 10; top: 10; right: 10; bottom: 10 }
    smooth: true
}

Rectangle {
    id:harect
    width: childrenRect.width
    height: childrenRect.height
    color: style.darkhighlight
    border.color: style.highlight1
    border.width: 3
    Column{
        id:controlcol
        height: childrenRect.height
        width: childrenRect.width
        spacing: scaleY(1)

        Remote_Audio_controls{}
        Remote_lighting_controls{}
    }
}
}
