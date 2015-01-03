import QtQuick 2.2

Item{
    anchors.fill: parent
    property int grpRadius:parent.radius ? parent.radius : 0
    property color fillColor:"black"

    Rectangle{
        anchors.fill: parent
        color: fillColor
        radius:grpRadius
        opacity:style.bgContentOpacity
    }
    Rectangle{
        anchors.fill: parent
        gradient: style.buttonGradient
        radius:grpRadius
        opacity: parent.opacity
    }
}


