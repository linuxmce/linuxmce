import QtQuick 2.2


Item{
    id:textrect
    visible: true
    height: childrenRect.height
    width: parent.width

    clip:true

    Column{
        id:textCol
        spacing: scaleY(.5)
        width: parent.width
        height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
        StyledText {
            id: starring_text
            horizontalAlignment: Text.AlignLeft
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: dcenowplaying.performerlist == "" ? dcenowplaying.performerlist : qsTr("No Performer Information")
            fontSize: scaleY(4)

            elide: "ElideRight"
            visible:false
        }
        StyledText {
            id: generaltitle
            width:parent.width
            text:  dcenowplaying.mediatitle
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
            elide: "ElideRight"
            smooth: true
            font.pixelSize: scaleY(4)
            visible:  dcenowplaying.mediatitle =="" ? false: true
        }
        StyledText { /* showing up with performers! fix */
            id: title_text
            width:parent.width
            horizontalAlignment: Text.AlignLeft
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
            id: album_text
            horizontalAlignment: Text.AlignLeft
            width:parent.width
            text: dcenowplaying.album
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.bold: true
            fontSize: scaleY(4)
        }

        StyledText {
            id: genre_text
            horizontalAlignment: Text.AlignLeft
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
            horizontalAlignment: Text.AlignLeft
            width:parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Released: ") + dcenowplaying.releasedate
            smooth: true
            font.bold: true
            fontSize: scaleY(4)
            visible:  dcenowplaying.releasedate =="" ? false: true
        }
    }
}
