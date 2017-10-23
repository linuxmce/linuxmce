import QtQuick 2.1

Item{
    anchors.fill: parent
    property int grpRadius:parent.radius ? parent.radius : 0
    property color fillColor:"black"

    Rectangle{
        anchors.fill: parent
        color: fillColor
        radius:grpRadius
        opacity:appstyle.bgContentOpacity
    }
    Rectangle{
        anchors.fill: parent
        gradient: appstyle.buttonGradient
        radius:grpRadius
        opacity: parent.opacity
    }
}


