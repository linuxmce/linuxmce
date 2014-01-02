import QtQuick 1.1

import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle{
    id:textrect
    visible: true
    height: parent.height
    width: scaleX(40)
    color:"transparent"
//    border.color: "black"
//    border.width: 1
    clip:true
    Column{
        id:textCol
        spacing: scaleY(.5)
        width: childrenRect.width
        height: parent.height
        StyledText {
            id: video_title
            width: scaleX(35)
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text:dcenowplaying.episode =="" ? qsTr("Channel: ")+ dcenowplaying.qs_mainTitle :qsTr("Program: ")+ dcenowplaying.tvProgram
            font.bold: true
            font.italic: false
            smooth: true
            fontSize: scaleY(4)
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true

        }
        StyledText {
            id: episode_title
            width: scaleX(35)
            text: qsTr("Program: ")+ dcenowplaying.tvProgram
            font.bold: true
            //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
            elide: "ElideRight"
            smooth: true
            font.pointSize: scaleY(3.5)
            visible:  dcenowplaying.tvProgram =="" ? false: true
            color:skinStyle.accentcolor
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
            color:skinStyle.accentColor
        }

        StyledText {
            id: synopsis_text
            width: scaleX(35)
            text: qsTr("Synopsis: ") + dcenowplaying.synop
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.bold: true
            fontSize: scaleY(2)
            color:"White"
            visible:  dcenowplaying.synop =="" ? false: true
        }

    }
}
