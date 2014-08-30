import QtQuick 2.1

Item{
    anchors.fill: parent
    property int grpRadius:parent.radius ? parent.radius : 0
    property color fillColor:"black"

    Rectangle{
        anchors.fill: parent
        color: fillColor
        radius:grpRadius
        opacity:appStyle.bgContentOpacity
    }
    Rectangle{
        anchors.fill: parent
        gradient: appStyle.buttonGradient
        radius:grpRadius
        opacity: parent.opacity
    }
}


