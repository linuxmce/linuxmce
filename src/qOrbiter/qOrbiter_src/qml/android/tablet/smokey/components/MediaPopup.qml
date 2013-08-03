import QtQuick 1.0

Item {
    id:media_notificaton_header
    width: parent.width
    height: scaleY(10)
    state:"hidden"

    Connections{
        target:dcenowplaying
        onB_mediaPlayingChanged:dcenowplaying.b_mediaPlaying && manager.currentScreen === "Screen_1.qml" ? media_notificaton_header.state ="active"  : media_notificaton_header.state = "hidden"

    }

    Connections{
        target:manager
        onCurrentScreenChanged:dcenowplaying.b_mediaPlaying && manager.currentScreen === "Screen_1.qml" ? media_notificaton_header.state ="active"  : media_notificaton_header.state = "hidden"

    }

    Rectangle{
        id:bg
        color:"green"
        anchors.fill:parent
        opacity: .65
    }

    Image {
        id: nowplayingimage

        height:parent.height
        fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.bottom:parent.bottom
        anchors.left: parent.left

        MouseArea{
            anchors.fill: parent
            onPressed: manager.gotoQScreen(dcenowplaying.qs_screen)
        }
    }
    Connections{
        target: dcenowplaying
        onImageChanged: refreshtimer.restart()
    }

    Timer{
        id:refreshtimer
        interval: 1000
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: nowplayingimage.visible
    }
    StyledText {
        id: generaltitle
        width: scaleX(35)
        text:  dcenowplaying.mediatitle === "" ? dcenowplaying.qs_mainTitle : dcenowplaying.mediatitle
        font.bold: true
        //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
        elide: "ElideRight"
        smooth: true
        color:"white"
        font.pixelSize: scaleY(4)
        anchors.top:parent.top
        anchors.left: nowplayingimage.right
        visible:nowplayingimage.height > scaleY(8)

    }

    StyledText {
        id: updating_time
        text: dceTimecode.qsCurrentTime + " of " + dceTimecode.qsTotalTime
        fontSize:32
        color: "white"
        anchors.left: nowplayingimage.right
        anchors.bottom: nowplayingimage.bottom

    }

    states: [
        State {
            name: "active"
         //   when:dcenowplaying.b_mediaPlaying ===true && manager.currentScreen ==="Screen_1.qml"

            PropertyChanges {
                target: media_notificaton_header
               height:scaleY(10)
            }
            AnchorChanges{
                target:media_notificaton_header
                anchors.top: nav_row.bottom
            }
        },
        State {
            name:"hidden"
         //   when:dcenowplaying.b_mediaPlaying ===false && manager.currentScreen !=="Screen_1.qml"

            PropertyChanges {
                target: media_notificaton_header
                height:0
            }
            AnchorChanges{
                target:media_notificaton_header
                anchors.top: qml_root.top
            }

        }
    ]



}
