import QtQuick 1.0
Item{
 height: style.stdbuttonh
 width: style.stdbuttonw



    Rectangle {
        id:harect
        width: childrenRect.width
        height: childrenRect.height
        color: "transparent"

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
