import QtQuick 1.0
import Qt.labs.shaders 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle{
    id:imageholder
    height:childrenRect.height
    width:scaleX(30)
    color: "transparent"   
    
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
        width: dcenowplaying.aspect=="wide"? scaleX(18) : scaleX(18)
        height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(30)
        source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
        anchors.horizontalCenter: parent.horizontalCenter
        smooth: true
    }
    
    Image {
        id: npmask
        source: "../img/icons/transparencymask.png"
        anchors.fill: nowplayingimage
        opacity: .5
    }
}
