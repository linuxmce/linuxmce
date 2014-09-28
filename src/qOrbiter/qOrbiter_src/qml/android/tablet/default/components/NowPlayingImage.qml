import QtQuick 1.0

Item{
    height: childrenRect.height
    width: childrenRect.width
    property int aspectRatio:dcenowplaying.aspect

    Connections{
        target:dcenowplaying
        onImageChanged:refreshtimer.start()
    }

    Image {
        id: nowplayingimage
        width: scaleY(35)
        height: scaleY(35)
       fillMode: Image.PreserveAspectFit
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

    states: [
        State {
            name: "wideImage"
            PropertyChanges {
                target: nowplayingimage

            }
        },
        State {
            name: "squareImage"
            PropertyChanges {
                target: nowplayingimage

            }
        },
        State{
            name:"posterImage"
            PropertyChanges{
                target: nowplayingimage
            }
        }

    ]
}
