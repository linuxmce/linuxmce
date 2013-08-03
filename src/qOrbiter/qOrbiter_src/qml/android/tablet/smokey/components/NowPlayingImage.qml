import QtQuick 1.0

Item{
    height: childrenRect.height
    width: childrenRect.width
    Connections{
        target:dcenowplaying
        onImageChanged:refreshtimer.start()
    }

    Image {
        id: nowplayingimage
        width:dcenowplaying.aspect==="album" ? scaleX(38) : dcenowplaying.aspect==="wide"? scaleX(50) : scaleX(25)
        height:dcenowplaying.aspect==="album" ? scaleY(45) : dcenowplaying.aspect==="wide"? scaleY(45) : scaleY(65)
       // fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
      //  anchors.horizontalCenter: parent.horizontalCenter
        smooth: true
        visible: source == undefined ? false : true
        onSourceChanged: {
            console.log("ASPECT==>"+dcenowplaying.aspect)
            console.log("Source Size ==>"+sourceSize)
        }
    }
    Timer{
        id:refreshtimer
        interval: 500
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: true
    }
}
