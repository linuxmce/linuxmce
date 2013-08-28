import QtQuick 1.0
import "../components"

Column{
    id:textcol
    spacing: scaleY(1.5)
    width: scaleX(80)
    height: childrenRect.height
    anchors.top: nowplayingimage.bottom
    anchors.topMargin: scaleY(4)
    anchors.horizontalCenter: parent.horizontalCenter
    
    Text {
        id: generaltitle
        width: parent.width
        text:  dcenowplaying.qs_mainTitle
        font.family: "Droid Sans"
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        font.bold: true
        smooth: true
        font.pixelSize: scaleY(4)
        visible:  dcenowplaying.qs_mainTitle =="" ? false: true
        color: "orange"
    }
    Text {
        id: tracktitle
        width: parent.width
        text:  dcenowplaying.mediatitle
        font.family: "Droid Sans"
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        smooth: true
        font.bold: true
        font.pixelSize: scaleY(3)
        visible:  dcenowplaying.mediatitle =="" ? false: true
        color: "silver"
    }
    
    Text {
        id: album
        width: parent.width
        text:  dcenowplaying.album + "|" + dcenowplaying.releasedate
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        font.bold: true
        smooth: true
        font.pixelSize: scaleY(2)
        visible:  dcenowplaying.album =="" ? false: true
        color: "silver"
    }
    
    
    Text {
        id: genre
        width: parent.width
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: qsTr("Genre(s): ") + dcenowplaying.genre
        font.family: "Droid Sans"
        font.bold: true
        smooth: true
        font.pixelSize: scaleY(2)
        visible:  dcenowplaying.genre =="" ? false: true
        color:"silver"
        MouseArea{
            anchors.fill: genre
            hoverEnabled: true
            onEntered: { genre.elide = "ElideNone" ; }
            onExited: {genre.elide = "ElideRight"; }
        }
    }
    
    Text {
        id: track
        
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        anchors.bottom:controls.top
        anchors.left: ha.right
        text: qsTr("Track: ") + dcenowplaying.track
        font.family: "Droid Sans"
        font.bold: true
        //font.italic: true
        smooth: true
        font.pixelSize: scaleY(3)
        visible:  dcenowplaying.track =="" ? false: true
        color: "orange"
        opacity: .50
    }
}
