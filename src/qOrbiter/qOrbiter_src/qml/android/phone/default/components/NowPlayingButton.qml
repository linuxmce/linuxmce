// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:np_box
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }
    height: manager.isProfile ? scaleY(8) : scaleY(12)
    visible: dcenowplaying.b_mediaPlaying ? true : false
    clip:true
    property real textSize:appStyle.buttonLabelText
    property variant propertyModel:{
        "title":dcenowplaying.mediatitle,
    }

    Rectangle{
        anchors.fill: parent
        color: "green"
        opacity:.65
    }
    Text {
        id: media_title
        anchors{
            left:parent.left
            verticalCenter: parent.verticalCenter
            right:np.left

        }
        elide:Text.ElideRight
        text:dcenowplaying.qs_mainTitle
        font.pixelSize: scaleY(4)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.weight:Font.DemiBold
        color: "white"
        visible:  dceTimecode.qsCurrentTime ==="00:00:00" && manager.i_current_mediaType ==4 ? false: true

    }


    Text {
        id: np
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }

        text:dceTimecode.qsCurrentTime
        font.pixelSize: scaleY(4)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.weight:Font.Light
        color: "white"
        visible:  dceTimecode.qsCurrentTime ==="00:00:00" && manager.i_current_mediaType ==4 ? false: true
    }



    //    Column{
    //        id:textcol
    //        spacing: scaleY(1)
    //        width: parent.width
    //        height: childrenRect.height
    //        anchors.top: np_box.top

    //        Text {
    //            id: genera_title
    //            width: parent.width
    //            text:  dcenowplaying.qs_mainTitle
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            smooth: true
    //            font.weight: Font.DemiBold
    //            font.pixelSize: scaleY(5)
    //            visible:  dcenowplaying.mediatitle ==="" ? true: false
    //            color: "white"
    //            elide: Text.ElideRight
    //        }

    //        Text {
    //            id: tracktitle
    //            width: parent.width
    //            text:  dcenowplaying.mediatitle
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            smooth: true
    //            font.bold: true
    //            font.pixelSize: scaleY(4)
    //            visible:  dcenowplaying.mediatitle =="" || dcenowplaying.tvProgram !== "" ? false: true
    //            color: "silver"
    //            elide: Text.ElideRight
    //        }
    //        Text {
    //            id: album
    //            width: parent.width
    //            text:  dcenowplaying.album + "|" + dcenowplaying.releasedate
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            smooth: true
    //            font.pixelSize: textSize
    //            visible:  dcenowplaying.album =="" ? false: true
    //            color: "white"
    //            elide: Text.ElideRight
    //        }
    //        Text {
    //            id: track
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            text: qsTr("Track: ") + dcenowplaying.track
    //            font.bold: true
    //            smooth: true
    //            font.pixelSize: textSize
    //            visible:  dcenowplaying.track =="" ? false: true
    //            color: "orange"
    //            opacity: .85
    //        }

    //        Text {
    //            id: programtext
    //            width: parent.width
    //            text: qsTr("Program :") + dcenowplaying.tvProgram
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            smooth: true
    //            font.pixelSize: textSize
    //            visible:  dcenowplaying.tvProgram ==="" ? false: true
    //            color:"orange"
    //        }

    //        Text {
    //            id: episode
    //            width: parent.width
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            text: qsTr("Episode: ") + dcenowplaying.episode
    //            smooth: true
    //            font.pixelSize: textSize
    //            visible:  dcenowplaying.episode ==="" ? false: true
    //            color:"white"
    //        }

    //        Text {
    //            id: genre
    //            width: parent.width
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            text: qsTr("Genre(s): ") + dcenowplaying.genre
    //            smooth: true
    //            font.pixelSize: textSize
    //            visible:  dcenowplaying.genre ==="" ? false: true
    //            color:"white"
    //            MouseArea{
    //                anchors.fill: genre
    //                hoverEnabled: true
    //                onEntered: { genre.elide = "ElideNone" ; }
    //                onExited: {genre.elide = "ElideRight"; }
    //            }
    //        }


    //        Text {
    //            id: starring
    //            width: manager.isProfile ? scaleX (65) : scaleX (25)
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            text: qsTr("Perfomers: ") + dcenowplaying.performerlist
    //            smooth: true
    //            font.pixelSize: textSize
    //            elide: "ElideRight"
    //            visible:  dcenowplaying.performerlist ==="" ? false: true
    //            color:"white"
    //            MouseArea{
    //                anchors.fill: starring
    //                hoverEnabled: true
    //                onEntered: { starring.elide = "ElideNone" ; }
    //                onExited: {starring.elide = "ElideRight"; }
    //            }
    //        }

    //        Text {
    //            id: network_id
    //            wrapMode: "NoWrap"
    //            text: qsTr("Network: ") + dcenowplaying.channelID
    //            font.bold: true
    //            smooth: true
    //            horizontalAlignment: Text.AlignHCenter
    //            font.pixelSize: textSize
    //            color: "white"
    //            visible:  dcenowplaying.channelID ==="" ? false: true
    //        }


    //        Text {
    //            id: channel_id
    //            wrapMode: "NoWrap"
    //            text: qsTr("Channel: ") + dcenowplaying.channel
    //            font.bold: true
    //            smooth: true
    //            horizontalAlignment: Text.AlignHCenter
    //            font.pixelSize: textSize
    //            color: "white"
    //            visible:  dcenowplaying.channel ==="" ? false: true
    //        }


    //        Text {
    //            id: np
    //            text:dceTimecode.qsCurrentTime
    //            font.pixelSize: scaleY(4)
    //            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    //            font.weight:Font.Light
    //            color: "white"
    //            visible:  dceTimecode.qsCurrentTime ==="00:00:00" && manager.i_current_mediaType ==4 ? false: true
    //        }

    //        /*  Text {
    //            id: synopsis
    //            width: parent.width
    //            wrapMode: "WrapAtWordBoundaryOrAnywhere"
    //            text: qsTr("Synopsis: ") + dcenowplaying.synop
    //            font.family: "Droid Sans"
    //            font.bold: true
    //            smooth: true
    //            font.pixelSize: scaleY(3)
    //            elide: "ElideRight"
    //            visible:  dcenowplaying.synop =="" ? false: true
    //            color:"silver"
    //            MouseArea{
    //                anchors.fill: synopsis
    //                hoverEnabled: true
    //                onEntered: { synopsis.elide = "ElideNone" ; }
    //                onExited: {synopsis.elide = "ElideRight"; }
    //            }
    //        }
    //        */

    //    }

    MouseArea{
        id: mousearea1
        anchors.fill: parent
        onClicked:manager.setCurrentScreen(dcenowplaying.qs_screen)
    }
}

