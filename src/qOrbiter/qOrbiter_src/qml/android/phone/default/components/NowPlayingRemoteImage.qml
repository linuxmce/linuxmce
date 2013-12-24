import QtQuick 1.1

Image {
    id: nowplayingimage
    property string aspectRatio:dcenowplaying.aspect
    height:manager.b_orientation? scaleX(55) : scaleX(25)
    width:manager.b_orientation ? scaleX(55)   : scaleY(25)

    fillMode: Image.PreserveAspectFit
    source: ""
    opacity: 1
    scale:1
    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter
    StyledText{
        anchors.centerIn: parent
        text:dcenowplaying.aspect
        fontSize: 18
    }
}
