import QtQuick 1.1



import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Column
{
    id:metadata
    height: scaleY(50)
    width: 480
    spacing: scaleY(1)
    Connections{
        target:dcenowplaying
        onImageChanged: {
            nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
            console.log("now playing changed")
        }
    }


    
    Image {
        id: nowplayingimage
        anchors.top:gradientheader.bottom
        anchors.topMargin: scaleY(.5)
        width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(20)
        height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(50)
        source: ""
        Component.onCompleted: manager.setBoundStatus(true)
    }
}
