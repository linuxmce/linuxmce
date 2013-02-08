import QtQuick 1.0
import Qt.labs.shaders 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle{
    id:textrect
    visible: true
    height: textCol.height
    width: scaleX(40)
    color:"transparent"

    clip:true

    Column{
        id:textCol
        spacing: scaleY(.5)
        width: childrenRect.width
        height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
        StyledText {
            id: video_title
            width: scaleX(35)
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: dcenowplaying.mediatitle
            font.bold: true
            font.italic: true
            smooth: true
            fontSize: scaleY(5)
            visible:  dcenowplaying.mediatitle =="" ? false: true
            opacity: .65
            style: Text.Sunken
        }
        StyledText {
            id: episode_title
            width: scaleX(35)
            text:  dcenowplaying.episode
            font.bold: true
            //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
            elide: "ElideRight"
            smooth: true
            font.pointSize: scaleY(3.5)
            visible:  dcenowplaying.episode =="" ? false: true
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
            color:style.maincolor
            visible:  dcenowplaying.synop =="" ? false: true
        }

    }
}
