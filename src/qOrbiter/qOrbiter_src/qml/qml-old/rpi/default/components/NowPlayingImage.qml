import QtQuick 2.2
import QtGraphicalEffects 1.0
import "../components"


Item{
    id:imageholder
    height:childrenRect.height
    width: childrenRect.height

    Connections{
        target: dcenowplaying
        onImageChanged: refreshtimer.restart()
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

    DropShadow{
    anchors.fill: nowplayingimage
    horizontalOffset: 3
           verticalOffset: 3
           radius: 8.0
           samples: 16
           color: "#80000000"
           source:nowplayingimage
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
