import QtQuick 1.1
import Qt.labs.shaders 1.0
Item{
    anchors.fill: parent
    property int grpRadius:parent.radius ? parent.radius : 0
    property color fillColor:appStyle.complimentColorDark
    Rectangle{
        anchors.fill: parent
        gradient: appStyle.buttonGradient
        radius:grpRadius
        opacity: parent.opacity
    }
    Rectangle{
        anchors.fill: parent
        color: fillColor
        radius:5
        opacity:parent.opacity
    }
}


