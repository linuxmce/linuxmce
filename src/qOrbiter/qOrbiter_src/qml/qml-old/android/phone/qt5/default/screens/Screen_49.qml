import QtQuick 2.2
import "../components"

Rectangle {

    // property alias synText:
    id: dvd_remote
    height: manager.appHeight
    width: manager.appWidth
    color: "transparent"
    Component.onCompleted:setNowPlayingData()

    NowPlayingBox
    {
        id:np_box
        anchors.top: parent.top
       anchors.left: home.right
       anchors.leftMargin: scaleX(5)
    }

    Column{
        id:textcol
        spacing: scaleY(1.5)
        width: scaleX(80)
        height: childrenRect.height
        anchors.top: np_box.bottom
        anchors.left: hacontrolbox.right
        anchors.leftMargin: scaleX(2)


        Text {
            id: generaltitle
            width: parent.width
            text:  dcenowplaying.qs_mainTitle
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true
            color: "orange"
        }

        Text {
            id: programtext
            width: parent.width
            text: qsTr("Program :") + dcenowplaying.tvProgram
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.tvProgram =="" ? false: true
            color:"silver"

        }

        Text {
            id: episode
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Episode: ") + dcenowplaying.episode
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.episode =="" ? false: true
            color:"silver"
        }

        Text {
            id: genre
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre
            font.family: "Droid Sans"
            // font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.genre =="" ? false: true
            color:"silver"
            MouseArea{
                anchors.fill: genre
                hoverEnabled: true
                onEntered: { genre.elide = "ElideNone" ; }
                onExited: {genre.elide = "ElideRight"; }
            }
        }


        Text {
            id: starring
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Perfomers: ") + dcenowplaying.performerlist
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            elide: "ElideRight"
            visible:  dcenowplaying.performerlist =="" ? false: true
            color:"silver"
            MouseArea{
                anchors.fill: starring
                hoverEnabled: true
                onEntered: { starring.elide = "ElideNone" ; }
                onExited: {starring.elide = "ElideRight"; }
            }
        }

        Text {
            id: synopsis
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Synopsis: ") + dcenowplaying.synop
            font.family: "Droid Sans"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            elide: "ElideRight"
            visible:  dcenowplaying.synop =="" ? false: true
            color:"silver"
            MouseArea{
                anchors.fill: synopsis
                hoverEnabled: true
                onEntered: { synopsis.elide = "ElideNone" ; }
                onExited: {synopsis.elide = "ElideRight"; }
            }
        }
    }


    AudioRemote{id:controls;anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter}

    //main 'now playing rect containing all the other items
    // Remote_lighting_controls{ id: remote_lighting_controls1; x: 344; y: 351; width: 93; height: 219; anchors.topMargin: 179;anchors.top: nowplayingbox1.baseline}
    // Remote_Audio_controls{ id: remote1; x: 200; y: 351; anchors.rightMargin: 71; z: 45; anchors.right: remote_lighting_controls1.left}
    HaControls{ id:hacontrolbox; anchors.left: parent.left; anchors.top: home.bottom; anchors.topMargin: scaleY(1)}
    HomeButton{ id:home; anchors.left: dvd_remote.left; anchors.top:dvd_remote.top; anchors.topMargin: scaleY(1)}


}

