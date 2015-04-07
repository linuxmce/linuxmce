import QtQuick 2.0
import "../components"
import "../js/ComponentLoader.js" as MyJs

Column{
    spacing: scaleY(.5)
    width: childrenRect.width
    height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)

    StyledText {
        id: starring
        width: scaleX(35)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: dcenowplaying.performerlist
        font.family:keyFont.name
        font.bold: true
        smooth: true
        font.pixelSize: scaleY(4)
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
        font.family: "Droid Sans"
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        smooth: true
        font.pixelSize: scaleY(3.5)
        visible:  dcenowplaying.mediatitle =="" ? false: true
    }
    
    StyledText {
        id: programtext
        width: scaleX(35)
        text: qsTr("Album: ") + dcenowplaying.album
        font.family: "Droid Sans"
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        smooth: true
        font.pixelSize: scaleY(2)
        visible:  dcenowplaying.album =="" ? false: true
    }
    
    StyledText {
        id: episode
        width: scaleX(35)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: qsTr("Track: ") + dcenowplaying.track
        font.family: "Droid Sans"
        //font.bold: true
        font.italic: true
        smooth: true
        font.pixelSize: scaleY(2)
        visible:  dcenowplaying.track =="" ? false: true
    }
    
    StyledText {
        id: genre
        width: scaleX(35)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: qsTr("Genre(s): ") + dcenowplaying.genre
        font.family: "Droid Sans"
        //font.bold: true
        font.italic: true
        smooth: true
        font.pixelSize: scaleY(2)
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
        font.family: "Droid Sans"
        // font.bold: true
        font.italic: true
        smooth: true
        font.pixelSize: scaleY(2)
        visible:  dcenowplaying.releasedate =="" ? false: true
        
    }
    
}
