// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:np_box
    width: scaleX(90)
    height: scaleY(35)
    color: "transparent"   
    visible: dcenowplaying.b_mediaPlaying ? true : false
    clip:true

    Column{
        id:textcol
        spacing: scaleY(1.5)
        width: parent.width
        height: childrenRect.height
        anchors.top: np_box.top

        Text {
            id: genera_title
            width: parent.width
            text:  dcenowplaying.qs_mainTitle
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.bold: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.mediatitle =="" ? true: false
            color: "silver"
        }

        Text {
            id: tracktitle
            width: parent.width
            text:  dcenowplaying.mediatitle
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.bold: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.mediatitle =="" ? false: true
            color: "silver"
        }
        Text {
            id: album
            width: parent.width
            text:  dcenowplaying.album + "|" + dcenowplaying.releasedate
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(2)
            visible:  dcenowplaying.album =="" ? false: true
            color: "silver"
        }
        Text {
            id: track

            wrapMode: "WrapAtWordBoundaryOrAnywhere"


            text: qsTr("Track: ") + dcenowplaying.track
            font.family: "Droid Sans"
            font.bold: true
            //font.italic: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.track =="" ? false: true
            color: "orange"
            opacity: .50
        }

        Text {
            id: programtext
            width: parent.width
            text: qsTr("Program :") + dcenowplaying.tvProgram
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(2)
            visible:  dcenowplaying.tvProgram ==="" ? false: true
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
            font.pixelSize: scaleY(2)
            visible:  dcenowplaying.episode ==="" ? false: true
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
            font.pixelSize: scaleY(2)
            visible:  dcenowplaying.genre ==="" ? false: true
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
            visible:  dcenowplaying.performerlist ==="" ? false: true
            color:"silver"
            MouseArea{
                anchors.fill: starring
                hoverEnabled: true
                onEntered: { starring.elide = "ElideNone" ; }
                onExited: {starring.elide = "ElideRight"; }
            }
        }

        Text {
            id: network_id
            wrapMode: "NoWrap"
            text: qsTr("Network: ") + dcenowplaying.channelID
            font.family: "Droid Sans"
            font.bold: true
            smooth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: scaleY(2)
            color: "white"
            visible:  dcenowplaying.channelID ==="" ? false: true
        }


        Text {
            id: channel_id
            wrapMode: "NoWrap"
            text: qsTr("Channel: ") + dcenowplaying.channel
            font.family: "Droid Sans"
            font.bold: true
            smooth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: scaleY(2)
            color: "white"
            visible:  dcenowplaying.channel ==="" ? false: true
        }


        Text {
            id: np
            text:dceTimecode.qsCurrentTime
            font.pixelSize: scaleY(2.5)            
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere

            color: "white"
            visible:  dceTimecode.qsCurrentTime ==="00:00:00" && dcerouter.i_current_mediaType ==4 ? false: true
        }

        /*  Text {
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
        */

    }

    MouseArea{
        id: mousearea1
        anchors.fill: parent
        onClicked:manager.setCurrentScreen(dcenowplaying.qs_screen)
    }
}

