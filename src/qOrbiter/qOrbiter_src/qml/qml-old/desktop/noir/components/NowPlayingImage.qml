import QtQuick 1.1
import "../components"


Rectangle{
    id:imageholder
    width: dcenowplaying.aspect=="wide"? scaleX(33) : scaleX(20)
    height:dcenowplaying.aspect=="wide"? scaleY(27) : scaleY(33)
    color: "transparent"
    border.color: "white"
    border.width: 2//activeFocus ? 2 : 0
    property bool largeSize:false


    Connections{
        target: dcenowplaying
        onImageChanged: refreshtimer.restart()
    }

    BorderImage {
        id: borderimg
        horizontalTileMode: BorderImage.Repeat
        source: "../lib/img/drpshadow.png"
        anchors.fill: nowplayingimage
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
        scale:nowplayingimage.scale
    }
    Image {
        id: nowplayingimage
        width: dcenowplaying.aspect=="wide"? scaleX(33) : scaleX(17)
        height:dcenowplaying.aspect=="wide"? scaleY(27) : scaleY(42)
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.centerIn: parent
        smooth: true
        visible: source == undefined ? false : true
        scale: largeSize ? 2 : 1
    }

    Timer{
        id:refreshtimer
        interval: 1500
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: true
    }

    Image {
        id: npmask
        source: "../lib/img/transparencymask.png"
        anchors.fill: nowplayingimage
        opacity: .5
        scale:nowplayingimage.scale
    }
}
