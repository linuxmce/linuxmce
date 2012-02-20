import QtQuick 1.0
Item{
 height: style.stdbuttonh
 width: style.stdbuttonw

    BorderImage {
        id: haborder
        horizontalTileMode: BorderImage.Repeat
        source: "../img/drpshadow.png"
        anchors.fill: harect
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }

    Rectangle {
        id:harect
        width: childrenRect.width
        height: childrenRect.height
        color: "transparent"
        border.color: "orange"
        border.width: 1
        Column{
            id:controlcol
            height: childrenRect.height
            width: childrenRect.width
            spacing: scaleY(1)
            Remote_lighting_controls{}
            Image {
                id: space
                source: "../img/line_4.png"
                width: parent.width
            }
            Remote_Audio_controls{}

        }
    }
}
