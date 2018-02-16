import QtQuick 2.2
Column{
    id:textCol
    spacing: scaleY(.5)
    width: scaleX(35)
    height: parent.height
    StyledText {
        id: generaltitle
        width: scaleX(45)
        text:  dcenowplaying.mediatitle
        font.bold: false
        //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
        elide: "ElideRight"
        smooth: true
        fontSize: 40
        visible:  dcenowplaying.mediatitle =="" ? false: true
        color:appstyle.title_color
    }
    StyledText { /* showing up with performers! fix */
        id: title_text
        width: scaleX(45)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: dcenowplaying.qs_mainTitle
        font.bold: false
        font.italic: true
        smooth: true
        fontSize: 32
        visible:  dcenowplaying.qs_mainTitle =="" ? false: true
        opacity: .65
        style: Text.Sunken
        color:appstyle.title_color
    }
    StyledText {
        id: starring_text
        width: scaleX(40)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: dcenowplaying.performerlist == "" ? dcenowplaying.performerlist : qsTr("No Performer Information")
        color:appstyle.title_color
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
        color:appstyle.sub_title_color
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
        color:appstyle.sub_title_color
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
        color:appstyle.sub_title_color
    }
}
