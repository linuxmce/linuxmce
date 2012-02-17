import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: storedVideoRemote
    height: appH
    width: appW
    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+Qt.formatTime
    }

    Connections{
        target:dcenowplaying
        onPlayListPositionChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        onImageChanged:nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }
    Component.onCompleted:setNowPlayingData()

    NowPlayingBox
    {
        id:np_box
       anchors.top: parent.top
    }


    Image{
        id:nowplayingimage
        fillMode: Image.PreserveAspectCrop
        source:"image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.fill: np_box
    }
    Column{
        id:textcol
        spacing: scaleY(1.5)
        width: scaleX(55)
        height: childrenRect.height
        anchors.top: np_box.bottom
        anchors.horizontalCenter: parent.horizontalCenter


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
        }

        Text {
            id: programtext
            width: parent.width
            text: qsTr("Program :") + dcenowplaying.tvProgram
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.tvProgram =="" ? false: true
        }

        Text {
            id: episode
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Episode: ") + dcenowplaying.episode
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.episode =="" ? false: true
        }

        Text {
            id: genre
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre
            font.family: "Droid Sans"
            // font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.genre =="" ? false: true
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
            font.pixelSize: scaleY(2)
            elide: "ElideRight"
            visible:  dcenowplaying.performerlist =="" ? false: true

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
            font.pixelSize: scaleY(1.5)
            elide: "ElideRight"
            visible:  dcenowplaying.synop =="" ? false: true

            MouseArea{
                anchors.fill: synopsis
                hoverEnabled: true
                onEntered: { synopsis.elide = "ElideNone" ; }
                onExited: {synopsis.elide = "ElideRight"; }
            }
        }
    }


    AudioRemote{id:controls;anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter}
    color: "transparent"

    //main 'now playing rect containing all the other items
    // Remote_lighting_controls{ id: remote_lighting_controls1; x: 344; y: 351; width: 93; height: 219; anchors.topMargin: 179;anchors.top: nowplayingbox1.baseline}
    // Remote_Audio_controls{ id: remote1; x: 200; y: 351; anchors.rightMargin: 71; z: 45; anchors.right: remote_lighting_controls1.left}
    HaControls{ id:hacontrolbox; anchors.left: parent.left; anchors.top: home.bottom}
    HomeButton{ id:home; anchors.left: storedVideoRemote.left; anchors.top:storedVideoRemote.top}


}

