import QtQuick 1.1
import "../../../skins-common/lib/handlers"
Column{
    id:textCol
    spacing: scaleY(.5)
    width: childrenRect.width
    height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
    visible: playlist.state === "hidden"
    StyledText {
        id: generaltitle
        width: scaleX(45)
        text:  dcenowplaying.mediatitle
        font.bold: true
        //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
        elide: "ElideRight"
        smooth: true
        fontSize: 40
        visible:  dcenowplaying.mediatitle =="" ? false: true
        color:skinstyle.accentcolor
    }
    StyledText { /* showing up with performers! fix */
        id: title_text
        width: scaleX(45)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: dcenowplaying.qs_mainTitle
        font.bold: true
        font.italic: true
        smooth: true
        fontSize: 32
        visible:  dcenowplaying.qs_mainTitle =="" ? false: true
        opacity: .65
        style: Text.Sunken
    }
    StyledText {
        id: starring_text
        width: scaleX(40)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: dcenowplaying.performerlist == "" ? dcenowplaying.performerlist : qsTr("No Performer Information")
        color: "Aliceblue"
        smooth: true
        fontSize: 32
        elide: "ElideRight"
        visible:false
    }
    
    StyledText {
        id: album_text
        width: scaleX(35)
        text: dcenowplaying.album
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        smooth: true
        font.bold: true
        fontSize: 32
        color:style.maincolor
    }
    
    StyledText {
        id: genre_text
        width: scaleX(35)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: qsTr("Genre(s): ") + dcenowplaying.genre
        smooth: true
        fontSize: 32
        font.bold: true
        visible:  dcenowplaying.genre =="" ? false: true
        
    }
    StyledText {
        id: released_text
        width: scaleX(35)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: qsTr("Released: ") + dcenowplaying.releasedate
        // font.italic: true
        smooth: true
        font.bold: true
        fontSize: 32
        visible:  dcenowplaying.releasedate =="" ? false: true
        color:skinstyle.accentColor
    }
}
