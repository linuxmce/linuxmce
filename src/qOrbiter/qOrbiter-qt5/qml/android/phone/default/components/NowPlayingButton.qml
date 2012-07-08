// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:np_box
    width: manager.b_orientation ? scaleX(90) : scaleX(40)
    height: manager.b_orientation ? scaleY(35) : scaleY(55)
    color: "transparent"
    visible: dcenowplaying.b_mediaPlaying ? true : false
    clip:true
    property real textSize:style.buttonLabelText
    Column{
        id:textcol
        spacing: scaleY(1)
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
            font.pixelSize: scaleY(4)
            visible:  dcenowplaying.mediatitle ==="" ? true: false
            color: "silver"
            elide: Text.ElideRight
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
            visible:  dcenowplaying.mediatitle =="" || dcenowplaying.tvProgram !== "" ? false: true
            color: "silver"
            elide: Text.ElideRight
        }
        Text {
            id: album
            width: parent.width
            text:  dcenowplaying.album + "|" + dcenowplaying.releasedate
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.bold: true
            smooth: true
            font.pixelSize: textSize
            visible:  dcenowplaying.album =="" ? false: true
            color: "silver"
            elide: Text.ElideRight
        }
        Text {
            id: track
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Track: ") + dcenowplaying.track
            font.family: "Droid Sans"
            font.bold: true
            //font.italic: true
            smooth: true
            font.pixelSize: textSize
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
            font.pixelSize: textSize
            visible:  dcenowplaying.tvProgram ==="" ? false: true
            color:"orange"
        }

        Text {
            id: episode
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Episode: ") + dcenowplaying.episode
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: textSize
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
            font.pixelSize: textSize
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
            width: manager.b_orientation ? scaleX (65) : scaleX (25)
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Perfomers: ") + dcenowplaying.performerlist
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: textSize
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
            font.pixelSize: textSize
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
            font.pixelSize: textSize
            color: "white"
            visible:  dcenowplaying.channel ==="" ? false: true
        }


        Text {
            id: np
            text:dceTimecode.qsCurrentTime
            font.pixelSize: scaleY(4)
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

