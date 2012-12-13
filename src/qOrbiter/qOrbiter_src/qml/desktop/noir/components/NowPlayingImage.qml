import QtQuick 1.1
import "../components"

Rectangle{
    id:imageholder
    height:childrenRect.height
    width:childrenRect.width
    color: "transparent"

    Connections{
        target: dcenowplaying
        onImageChanged: refreshtimer.restart()
    }

    BorderImage {
        id: borderimg
        horizontalTileMode: BorderImage.Repeat
        source: "../img/drpshadow.png"
        anchors.fill: nowplayingimage
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }
    Image {
        id: nowplayingimage
        width: dcenowplaying.aspect=="wide"? scaleX(33) : scaleX(25)
        height:dcenowplaying.aspect=="wide"? scaleY(27) : scaleY(33)
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.horizontalCenter: parent.horizontalCenter
        smooth: true
        visible: source == undefined ? false : true
    }

    Timer{
        id:refreshtimer
        interval: 1500
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: true
    }

    Image {
        id: npmask
        source: "../img/transparencymask.png"
        anchors.fill: nowplayingimage
        opacity: .5
    }
}
