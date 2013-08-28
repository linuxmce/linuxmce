import QtQuick 1.1

Image {
    id: nowplayingimage
    height: scaleX(55)
    width: scaleX(55)   
    fillMode: Image.PreserveAspectFit
    source: ""
    opacity: 1
    scale:1
    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter
}
