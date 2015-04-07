import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs


Item{
    id:imageholder
    height:nowplayingimage.height
    width:nowplayingimage.width

    
    BorderImage {
        id: borderimg
        horizontalTileMode: BorderImage.Repeat
        source: "../img/icons/drpshadow.png"
        anchors.fill: nowplayingimage
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }
    Image {
        id: nowplayingimage
        width: dcenowplaying.aspect=="wide"? scaleX(40) : scaleX(40)
        height:dcenowplaying.aspect=="wide"? scaleY(55) : scaleY(50)
        source:nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.horizontalCenter: parent.horizontalCenter
        smooth: true
        Connections{
            target: dcenowplaying
            onImageChanged:{nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp; console.log(dcenowplaying.m_iplaylistPosition)}
        }
        Timer{
            interval: 1000
            running:true
            onTriggered: nowplayingimage.source="image://listprovider/updateobject/"+securityvideo.timestamp
        }

    }
    
    Image {
        id: npmask
        source: "../img/icons/transparencymask.png"
        anchors.fill: nowplayingimage
        opacity: .5
    }
}
