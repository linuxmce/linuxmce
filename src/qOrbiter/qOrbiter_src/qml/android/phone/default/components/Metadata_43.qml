import QtQuick 1.0
import "../components"

Flickable{
    height:scaleY(35)
    width: textcol.width
    anchors.top: np_box.bottom
    anchors.horizontalCenter: np_box.horizontalCenter
    contentHeight: textcol.height
    clip:true
    boundsBehavior: Flickable.DragOverBounds
    Column{
        id:textcol
        spacing: scaleY(1.5)
        width: scaleX(90)
        height: childrenRect.height

        StyledText {
            id: generaltitle
            width: parent.width
            text:  dcenowplaying.qs_mainTitle
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true
            color: "orange"

        }


        StyledText {
            id: genre
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.genre =="" ? false: true
            color:"silver"
            MouseArea{
                anchors.fill: genre
                hoverEnabled: true
                //     onEntered: { genre.elide = "ElideNone" ; }
                //     onExited: {genre.elide = "ElideRight"; }
            }

        }


        StyledText {
            id: starring
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Perfomers: ") + dcenowplaying.performerlist
            smooth: true
            font.pixelSize: scaleY(3)
            // elide: "ElideRight"
            visible:  dcenowplaying.performerlist =="" ? false: true
            color:"silver"
            MouseArea{
                anchors.fill: starring
                hoverEnabled: true
                //onEntered: { starring.elide = "ElideNone" ; }
                // onExited: {starring.elide = "ElideRight"; }
            }
        }


    }
}
