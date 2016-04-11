import QtQuick 1.0

Rectangle{
    id:controlcol
    height: childrenRect.height
    width: style.stdbuttonW

    Remote_lighting_controls{
        id:lc
        anchors.top: parent.top
    }
    Image {
        id: space
        source: "../img/line_4.png"
        width: parent.width
        anchors.top: lc.bottom
    }
    Remote_Audio_controls{
        id:ac
        anchors.top: space.bottom
    }

}


