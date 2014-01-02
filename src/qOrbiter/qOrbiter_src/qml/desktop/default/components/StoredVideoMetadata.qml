import QtQuick 1.1



import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle{
    id:textrect
    height: childrenRect.height
    width: scaleX(25)
    color:"transparent"

    /*
            BorderImage {
                id: metaborder
                horizontalTileMode: BorderImage.Repeat
                source: "../../../img/icons/drpshadow.png"
                anchors.fill: textrect
                anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                border { left: 10; top: 10; right: 10; bottom: 10 }
                smooth: true
            }
*/
    z:10
    Column{
        id:textcol
        spacing: scaleY(1.5)
        width: parent.width
        height: childrenRect.height
        
        
        Text {
            id: generaltitle
            width: parent.width
            text:  dcenowplaying.qs_mainTitle
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true
        }
        
        Text {
            id: programtext
            width: parent.width
            text: qsTr("Program :") + dcenowplaying.tvProgram
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.tvProgram =="" ? false: true
        }
        
        Text {
            id: episode
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Episode: ") + dcenowplaying.episode
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.episode =="" ? false: true
        }
        
        Text {
            id: genre
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre
            font.family: "Droid Sans"
            // font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.genre =="" ? false: true
            MouseArea{
                anchors.fill: genre
                hoverEnabled: true
                onEntered: { genre.elide = "ElideNone" ; }
                onExited: {genre.elide = "ElideRight"; }
            }
        }
        
        
        Text {
            id: starring
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Perfomers: ") + dcenowplaying.performerlist
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(2)
            elide: "ElideRight"
            visible:  dcenowplaying.performerlist =="" ? false: true
            
            MouseArea{
                anchors.fill: starring
                hoverEnabled: true
                onEntered: { starring.elide = "ElideNone" ; }
                onExited: {starring.elide = "ElideRight"; }
            }
        }
        
        Text {
            id: synopsis
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Synopsis: ") + dcenowplaying.synop
            font.family: "Droid Sans"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            elide: "ElideRight"
            visible:  dcenowplaying.synop =="" ? false: true
            
            MouseArea{
                anchors.fill: synopsis
                hoverEnabled: true
                onEntered: { synopsis.elide = "ElideNone" ; }
                onExited: {synopsis.elide = "ElideRight"; }
            }
        }
        
        Text {
            id: path
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Path: ") + dcenowplaying.path
            font.family: "Droid Sans"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            elide: "ElideRight"
            visible:  dcenowplaying.path =="" ? false: true
            
            MouseArea{
                anchors.fill: path
                hoverEnabled: true
                onEntered: { path.elide = "ElideNone" ; }
                onExited: {path.elide = "ElideRight"; }
            }
        }
        
        Text {
            id: sd
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Storage Device: ") + dcenowplaying.qs_storageDevice
            font.family: "Droid Sans"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            elide: "ElideRight"
            visible:  dcenowplaying.qs_storageDevice =="" ? false: true
            
            MouseArea{
                anchors.fill: sd
                hoverEnabled: true
                onEntered: { sd.elide = "ElideNone" ; }
                onExited: {sd.elide = "ElideRight"; }
            }
        }
    }
}
