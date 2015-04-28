import QtQuick 2.2

Item {
    id:media_notificaton_header
    width: parent.width
    height: scaleY(10)
    Component.onCompleted: state="hidden"


    Rectangle{
        id:bg
        color:"green"
        anchors.fill:parent
        opacity: .65
        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(!uiOn){
                    uiOn=true
                    manager.setCurrentScreen(dcenowplaying.qs_screen)
                }
            }
        }
    }

    Image {
        id: nowplayingimage

        height:parent.height
        fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.bottom:parent.bottom
        anchors.left: parent.left
        sourceSize: Qt.size(200,300)

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
        visible:dcenowplaying.b_mediaPlaying

    }

    StyledText {
        id: updating_time
        text: dceTimecode.qsCurrentTime + " of " + dceTimecode.qsTotalTime
        fontSize:32
        color: "white"
        anchors.left: nowplayingimage.right
        anchors.bottom: nowplayingimage.bottom
        visible:generaltitle.visible
    }

    Clock{
        id:time_keeper
        anchors{
            left:parent.right
            verticalCenter: parent.verticalCenter
        }
        opacity: .65
        boldClock: true
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            if(!uiOn){
                uiOn=true
            }

            if(dcenowplaying.b_mediaPlaying)
             manager.setCurrentScreen(dcenowplaying.qs_screen)
        }
    }
    states: [

        State {
            name: "active"
            when:dcenowplaying.b_mediaPlaying && manager.currentScreen === "Screen_1.qml" && uiOn
            //   when:dcenowplaying.b_mediaPlaying ===true && manager.currentScreen ==="Screen_1.qml"
            PropertyChanges{
                target: media_notificaton_header
                height:scaleY(10)
            }
            AnchorChanges{
                target:media_notificaton_header
                anchors.bottom:undefined
                anchors.top: nav_row.bottom
            }
            PropertyChanges{
                target: nowplayingimage
                visible:true
            }
        },
        State {
            name:"hidden"
            when:(manager.currentScreen !== "Screen_1.qml" || dcenowplaying.b_mediaPlaying ===false ) && uiOn
            //   when:dcenowplaying.b_mediaPlaying ===false && manager.currentScreen !=="Screen_1.qml"
            PropertyChanges{
                target: nowplayingimage
                visible:false
            }
            AnchorChanges{
                target:media_notificaton_header
                anchors.top: undefined
                anchors.bottom: qml_root.top
            }
        },
        State{
            name:"media-screensaver"
            when:!uiOn && dcenowplaying.b_mediaPlaying

            PropertyChanges {
                target: bg
                opacity:.25
                color:"black"
            }
            PropertyChanges{
                target: nowplayingimage
                visible:true
            }

            AnchorChanges{
                target:media_notificaton_header
                anchors.top: qml_root.top
                anchors.bottom: qml_root.bottom
            }
        },
        State{
            name:"screensaver"
            when:!uiOn && !dcenowplaying.b_mediaPlaying

            PropertyChanges{
                target: nowplayingimage
                visible:false
            }

            PropertyChanges {
                target: bg
                opacity:.25
                color:"black"
            }

            AnchorChanges{
                target:media_notificaton_header
                anchors.top: qml_root.top
                anchors.bottom: qml_root.bottom
            }

            AnchorChanges{
                target: time_keeper
                anchors{
                    left:undefined
                    right:parent.right
                }
            }
        }

    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration:skinStyle.animation_medium
                easing.type: skinStyle.animation_easing
            }

        }
    ]
}
