import QtQuick 1.0
import "../components"

Column{
    id:textcol
    spacing: scaleY(1.5)
    width: scaleX(100)
    height: childrenRect.height+(scaleY(2))

    Text {
        id: performer
        width: parent.width
        text: qsTr("Artist: ")+ dcenowplaying.performerlist
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        elide: Text.ElideRight
        smooth: true
        font.pixelSize: scaleY(5)
        visible:  dcenowplaying.performerlist =="" ? false: true
        color: "white"
    }

    Text {
        id: generaltitle
        width: parent.width
        text:  dcenowplaying.qs_mainTitle
         elide: Text.ElideRight
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        smooth: true
        font.pixelSize: scaleY(5)
        visible:  dcenowplaying.qs_mainTitle =="" ? false: true
        color: "white"
    }

    Text {
        id: tracktitle
        width: parent.width
         elide: Text.ElideRight
        text: qsTr("Title: ") + dcenowplaying.mediatitle
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        font.pixelSize: scaleY(4)
        visible:  dcenowplaying.mediatitle =="" ? false: true
        color: "white"
    }

    Text {
        id: album
        width: parent.width
        text: qsTr("Album: ")+ dcenowplaying.album + " | " + dcenowplaying.releasedate
         elide: Text.ElideRight
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        smooth: true
        font.pixelSize: scaleY(4)
        visible:  dcenowplaying.album =="" ? false: true
        color: "white"
    }


    Text {
        id: genre
        width: parent.width
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        text: qsTr("Genre(s): ") + dcenowplaying.genre
         elide: Text.ElideRight
        smooth: true
        font.pixelSize: scaleY(4)
        visible:  dcenowplaying.genre =="" ? false: true
        color:"white"
        MouseArea{
            anchors.fill: genre
            hoverEnabled: true
            onEntered: { genre.elide = "ElideNone" ; }
            onExited: {genre.elide = "ElideRight"; }
        }
    }

    Text {
        id: track
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        anchors.bottom:controls.top
        anchors.left: ha.right
        text: qsTr("Track: ") + dcenowplaying.track
        //font.italic: true
        smooth: true
        font.pixelSize: scaleY(5)
        visible:  dcenowplaying.track =="" ? false: true
        color: "orange"
        opacity: .50
    }
}
