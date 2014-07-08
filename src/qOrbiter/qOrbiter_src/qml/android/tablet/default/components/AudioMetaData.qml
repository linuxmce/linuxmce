import QtQuick 1.0
import Qt.labs.shaders 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle{
    id:textrect
    visible: true
    height: childrenRect.height
    width: scaleX(40)
    color:"transparent"   

    clip:true
    StyledText {
        id: episode
        width: scaleX(35)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: dcenowplaying.track
                   font.bold: true
        font.italic: true
        smooth: true
        fontSize: scaleY(15)
        visible:  dcenowplaying.track =="" ? false: true
        anchors.centerIn: textCol
        opacity: .65
        style: Text.Sunken
    }
    Column{
        id:textCol
        spacing: scaleY(.5)
        width: childrenRect.width
        height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
        StyledText {
            id: starring
            width: scaleX(40)
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: dcenowplaying.performerlist
            color: "Aliceblue"
          //  font.bold: true
            smooth: true
            fontSize: scaleY(4)
            elide: "ElideRight"
            visible:  dcenowplaying.performerlist =="" ? false: true
            
            MouseArea{
                anchors.fill: starring
                hoverEnabled: true
                onEntered: { starring.elide = "ElideNone" ; }
                onExited: {starring.elide = "ElideRight"; }
            }
        }
        StyledText {
            id: generaltitle
            width: scaleX(35)
            text:  dcenowplaying.mediatitle
            font.bold: true
          //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
             elide: "ElideRight"
            smooth: true
            font.pixelSize: scaleY(3.5)
            visible:  dcenowplaying.mediatitle =="" ? false: true
            color:appStyle.accentcolor
        }
        
        StyledText {
            id: programtext
            width: scaleX(35)
            text: qsTr("Album: ") + dcenowplaying.album
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.bold: true
           fontSize: scaleY(3.5)
           color:style.maincolor
            visible:  dcenowplaying.album =="" ? false: true
        }
        

        
        StyledText {
            id: genre
            width: scaleX(35)
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre           
            smooth: true
            fontSize: scaleY(2)
             font.bold: true
            visible:  dcenowplaying.genre =="" ? false: true
            MouseArea{
                anchors.fill: genre
                hoverEnabled: true
                onEntered: { genre.elide = "ElideNone" ; }
                onExited: {genre.elide = "ElideRight"; }
            }
        }
        StyledText {
            id: released
            width: scaleX(35)
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Released: ") + dcenowplaying.releasedate          
           // font.italic: true

            smooth: true
             font.bold: true
            fontSize: scaleY(2)
            visible:  dcenowplaying.releasedate =="" ? false: true
            color:appStyle.accentColor
        }
        
    }
}
