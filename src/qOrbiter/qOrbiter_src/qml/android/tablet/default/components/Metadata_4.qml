import QtQuick 1.0


Item{
    id:textrect
    visible: true
    height: childrenRect.height
    width: parent.width
    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity:.65
    }
    clip:true

    Column{
        id:textCol
        spacing: scaleY(.5)
        width: childrenRect.width
        height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
        StyledText {
            id: generaltitle
            width:parent.width
            text:  dcenowplaying.mediatitle
            font.bold: true
            //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
            elide: "ElideRight"
            smooth: true
            font.pixelSize: scaleY(4)
            visible:  dcenowplaying.mediatitle =="" ? false: true
            color:appStyle.accentcolor
        }
        StyledText { /* showing up with performers! fix */
            id: title_text
            width:parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: dcenowplaying.qs_mainTitle
            font.bold: true
            font.italic: true
            smooth: true
            fontSize: scaleY(4)
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
            //  font.bold: true
            smooth: true
            fontSize: scaleY(4)
            elide: "ElideRight"
            visible:false
        }

        StyledText {
            id: album_text
            width:parent.width
            text: dcenowplaying.album
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.bold: true
            fontSize: scaleY(4)
            color:style.maincolor
        }

        StyledText {
            id: genre_text
            width:parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre
            smooth: true
            fontSize: scaleY(4)
            font.bold: true
            visible:  dcenowplaying.genre =="" ? false: true

        }
        StyledText {
            id: released_text
            width:parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Released: ") + dcenowplaying.releasedate
            // font.italic: true

            smooth: true
            font.bold: true
            fontSize: scaleY(4)
            visible:  dcenowplaying.releasedate =="" ? false: true
            color:appStyle.accentColor
        }
    }
}
