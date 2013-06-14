import QtQuick 2.0

Item{
    height: childrenRect.height
    width: childrenRect.width
    Image {
        id: nowplayingimage
        width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(25)
        height:dcenowplaying.aspect=="wide"? scaleY(43) : scaleY(45)
        fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.horizontalCenter: parent.horizontalCenter
        smooth: true
        visible: source == undefined ? false : true
    }
    Timer{
        id:refreshtimer
        interval: 1000
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: true
    }
}
