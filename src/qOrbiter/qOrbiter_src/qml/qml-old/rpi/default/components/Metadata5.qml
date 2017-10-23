import QtQuick 2.2

Column{
    id:textCol
    spacing: scaleY(.5)
    width: childrenRect.width
    height: parent.height
    StyledText {
        id: video_title
        width: scaleX(35)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text:dcenowplaying.episode =="" ? qsTr("Title: ")+ dcenowplaying.qs_mainTitle :qsTr("Program: ")+ dcenowplaying.tvProgram
        font.bold: false
        font.italic: false
        smooth: true
        fontSize: scaleY(6)
        visible:  dcenowplaying.qs_mainTitle =="" ? false: true
        color:"white"

    }
    StyledText {
        id: episode_title
        width: scaleX(35)
        text: qsTr("Episode: ")+ dcenowplaying.episode
        font.bold: false
        //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
        elide: "ElideRight"
        smooth: true
        fontSize: scaleY(4)
        visible:  dcenowplaying.episode =="" ? false: true
        color:skinstyle.accentcolor
    }
    //    StyledText {
    //        id: starring
    //        width: scaleX(40)
    //        wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //        text: dcenowplaying.performerlist
    //        color: "Aliceblue"
    //        //  font.bold: true
    //        smooth: true
    //        fontSize: scaleY(4)
    //        elide: "ElideRight"
    //        visible:  dcenowplaying.performerlist =="" ? false: true
    ////            MouseArea{
    ////                anchors.fill: starring
    ////                hoverEnabled: true
    ////                onEntered: { starring.elide = "ElideNone" ; }
    ////                onExited: {starring.elide = "ElideRight"; }
    ////            }
    //    }

    //    StyledText {
    //        id: genre
    //        width: scaleX(35)
    //        wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //        text: qsTr("Genre(s): ") + dcenowplaying.genre
    //        smooth: true
    //        fontSize: scaleY(2)
    //        font.bold: true
    //        visible:  dcenowplaying.genre =="" ? false: true
    //        MouseArea{
    //            anchors.fill: genre
    //            hoverEnabled: true
    //            onEntered: { genre.elide = "ElideNone" ; }
    //            onExited: {genre.elide = "ElideRight"; }
    //        }
    //    }

    StyledText {
        id: released
        width: scaleX(35)
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: qsTr("Released: ") + dcenowplaying.releasedate
        // font.italic: true
        smooth: true
        font.bold: true
        fontSize: scaleY(4)
        visible:  dcenowplaying.releasedate =="" ? false: true
        color:"white"
    }

    //    StyledText {
    //        id: synopsis_text
    //        width: scaleX(35)
    //        text: qsTr("Synopsis: ") + dcenowplaying.synop
    //        wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //        smooth: true
    //        font.bold: true
    //       fontSize: scaleY(4)
    //        color:"White"
    //        visible:  dcenowplaying.synop =="" ? false: true
    //    }

}
