import QtQuick 1.0

Item{
    id:textrect
    visible: true
    height: textCol.height
    width: parent.width
    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity:.65
    }

    //    border.color: "black"
    //    border.width: 1
    clip:true
    Column{
        id:textCol
        spacing: scaleY(.5)
        width: parent.width
        anchors.left: parent.left
        anchors.leftMargin: scaleY(1)
        height: childrenRect.height
        StyledText {
            id: video_title
            width:parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text:dcenowplaying.episode =="" ? qsTr("Title: ")+ dcenowplaying.qs_mainTitle :qsTr("Program: ")+ dcenowplaying.tvProgram
            font.italic: false
            smooth: true
            fontSize: largeFontSize
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true

        }
        StyledText {
            id: episode_title
            width:parent.width
            text: qsTr("Episode: ")+ dcenowplaying.episode
            //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
            elide: "ElideRight"
            smooth: true
            fontSize: scaleY(4)
            visible:  dcenowplaying.episode =="" ? false: true
            // color:appstyle.accentcolor
        }
        StyledText {
            id: starring
            width: scaleX(40)
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: dcenowplaying.performerlist
            color: "Aliceblue"
            //  font.bold: true
            smooth: true
            fontSize: scaleY(3)
            elide: "ElideRight"
            visible:  dcenowplaying.performerlist =="" ? false: true
            //            MouseArea{
            //                anchors.fill: starring
            //                hoverEnabled: true
            //                onEntered: { starring.elide = "ElideNone" ; }
            //                onExited: {starring.elide = "ElideRight"; }
            //            }
        }
        StyledText {
            id: genre
            width:parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre
            smooth: true
            fontSize: scaleY(3)

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
            width:parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Released: ") + dcenowplaying.releasedate
            // font.italic: true
            smooth: true
            fontSize: scaleY(4)
            visible:  dcenowplaying.releasedate =="" ? false: true
            // color:appstyle.accentColor
        }

        StyledText {
            id: synopsis_text
            width:parent.width
            text: qsTr("Synopsis: ") + dcenowplaying.synop
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.bold: true
            fontSize: scaleY(3)
            color:"White"
            visible:  dcenowplaying.synop =="" ? false: true
        }

    }
}
