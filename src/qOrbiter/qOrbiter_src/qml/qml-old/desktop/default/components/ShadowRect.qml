import QtQuick 1.1
//drop shadow rectangle component for display panel
Item{
    property alias rectcolor: shadowrectcontainer.color
    property alias rectheight:shadowrectcontainer.height
    property alias rectwidth: shadowrectcontainer.width



BorderImage {
    id: borderimg
    horizontalTileMode: BorderImage.Repeat
    source: "../img/icons/drpshadow.png"
    anchors.fill: shadowrectcontainer
    anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
    border { left: 10; top: 10; right: 10; bottom: 10 }
    smooth: true
}

Rectangle {
    id:shadowrectcontainer
    anchors.centerIn: parent
    height:scaleY(55)
    width: scaleX(55)
    color:"transparent"
    radius: 2.5
    smooth: true
}

}
