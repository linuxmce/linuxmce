import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: storedAudioRemote
    height: manager.appHeight
    width: manager.appWidth
    color: "transparent"

    Connections
     {
         target: dcenowplaying
         onImageChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
     }



    Component.onCompleted: manager.setBoundStatus(true)

    function changeTracksTransition()
    {
        if (nowplayingimage.source === "")
        {
            img_add.running = true
        }
        else
        {
            img_remove.running = true
        }
    }

    ParallelAnimation {
        id:img_add
        running: false
        PropertyAnimation { target: nowplayingimage; property: "scale"; to: 1; duration: 1500}
         PropertyAnimation { target: nowplayingimage; property: "opacity"; to: 1; duration: 1500}
    }

    ParallelAnimation {
        id:img_remove
        running:false
        PropertyAction { target: nowplayingimage; property: "scale"; value: 0 }

        onCompleted: img_add.running = true
    }

    Image {
       id: nowplayingimage
       height: scaleX(55)
       width: scaleX(55)
       fillMode: Image.PreserveAspectFit
       source: ""
       opacity: 0
       scale:0
       onSourceChanged: changeTracksTransition()
       anchors.top: parent.top
       anchors.horizontalCenter: parent.horizontalCenter
   }

    Column{
        id:textcol
        spacing: scaleY(1.5)
        width: scaleX(80)
        height: childrenRect.height
        anchors.top: nowplayingimage.bottom
        anchors.topMargin: scaleY(4)
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: generaltitle
            width: parent.width
            text:  dcenowplaying.qs_mainTitle
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(4)
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true
            color: "orange"
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
            id: genre
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre
            font.family: "Droid Sans"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(2)
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
            id: track

            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            anchors.bottom:controls.top
            anchors.left: ha.right
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
    }

    HomeButton{ id:home ;anchors.left: parent.left; anchors.top: parent.top}

    ButtonSq {
        id: buttonsq1
        anchors.top: home.bottom
        anchors.left: storedAudioRemote.left
        height: style.stdbuttonh
        width: style.stdbuttonw
        buttontext: qsTr("Playlist")
        MouseArea{
            anchors.fill: parent
            onClicked: storedAudioRemote.state = "State1"
        }
    }

    ButtonSq {
        id: metadata
        anchors.top: home.bottom
        anchors.left: storedAudioRemote.left
        height: style.stdbuttonh
        width: style.stdbuttonw
        buttontext: qsTr("Metadata")
        visible: false
        MouseArea{
            anchors.fill: parent
            onClicked: storedAudioRemote.state = ""
        }
    }
    Text {
        id: np
        text:dceTimecode.qsCurrentTime
        font.pixelSize: scaleY(2.5)
        anchors.bottom: media_scroller.top
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.horizontalCenter: media_scroller.horizontalCenter
        color: "white"
        visible:  dceTimecode.qsCurrentTime ==="0" ? false: true
    }
    MediaScrollBar{id:media_scroller; anchors.bottom: controls.top; anchors.horizontalCenter: parent.horizontalCenter; anchors.bottomMargin: scaleY(2)}
    HaControls{id:ha; anchors.left: parent.left ; anchors.top: buttonsq1.bottom }
    AudioRemote{ id:controls; anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter}
    NonEPGPlaylist {
        id: nonepgplaylist3
        anchors.top: storedAudioRemote.top
        anchors.topMargin: scaleY(4)
        anchors.right: parent.right
        visible: false
        opacity: 0
    }

states: [
State {
    name: "State1"

    PropertyChanges {
        target: nonepgplaylist3
        visible: true
        opacity: 1
    }

    PropertyChanges {
        target: textcol
        visible: false
    }

    PropertyChanges {
        target: nowplayingimage
        visible: false
    }

    PropertyChanges {
        target: metadata
        visible: true
    }

    PropertyChanges {
        target: buttonsq1
        visible: false
    }
}
]
}





