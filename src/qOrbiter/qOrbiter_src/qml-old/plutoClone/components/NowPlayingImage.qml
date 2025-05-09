import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../components"
import "../../../skins-common/lib/effects"

Rectangle{
    id:imageholder
    height: playlist.state==="showing" ? childrenRect.height : 0
    width: playlist.state === "showing" ? scaleX(30) :0
    color: "transparent"
    Connections{
        target: dcenowplaying
        onImageChanged: refreshtimer.restart()
    }
    
    //    BorderImage {
    //        id: borderimg
    //        horizontalTileMode: BorderImage.Repeat
    //        source: "../img/icons/drpshadow.png"
    //        height: nowplayingimage.paintedHeight
    //        width: nowplayingimage.paintedWidth
    //        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
    //        border { left: 10; top: 10; right: 10; bottom: 10 }
    //        smooth: true
    //    }
    
    
    DropShadow{
        sourceItem: nowplayingimage
        distance: 4
        color:"black"
        anchors.fill: sourceItem
        visible:nowplayingimage.visible
    }
    Image {
        id: nowplayingimage
        width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(25)
        height:dcenowplaying.aspect=="wide"? scaleY(43) : scaleY(45)
        fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.horizontalCenter: parent.horizontalCenter
        smooth: true
        visible: playlist.state ==="showing" ? source == undefined ? false : true :false
    }
    Timer{
        id:refreshtimer
        interval: 1000
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: true
    }
    
    Image {
        id: npmask
        source: "../img/icons/transparencymask.png"
        height: nowplayingimage.paintedHeight
        width: nowplayingimage.paintedWidth
        anchors.centerIn: nowplayingimage
        opacity: .75
    }
}
