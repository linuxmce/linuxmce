import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: storedaudioremote

    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true

        onTriggered: nowplayingimage.source = "image://updateobject/"+securityvideo.timestamp
    }

    Connections{
        target:dcenowplaying
        onPlayListPositionChanged: nowplayingimage.source = "image://updateobject/"+securityvideo.timestamp
    }

    height: style.orbiterH
    width: style.orbiterW
    radius: 0
    opacity: 1
    color: style.darkhighlight
    Component.onCompleted:setNowPlayingData()
    //main 'now playing rect containing all the other items

    Row{
        id:mainrow
        height: childrenRect.height
        width: childrenRect.width
        spacing: 1
        anchors.top:parent.top
        anchors.topMargin: scaleY(5)
        anchors.horizontalCenter: parent.horizontalCenter

        NonEPGPlaylist{ id:playlist}

        Row {
            width: childrenRect.width
            height: childrenRect.height
            spacing: 1

            Column
            {
                height: childrenRect.height
                width: childrenRect.width

                Rectangle {
                id: gradientheader
                width: parent.width
                height: childrenRect.height
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#ffffff"
                    }

                    GradientStop {
                        position: 1
                        color: "#3878a0"
                    }
                }
                Text {
                    id: headertext
                    text:qsTr("Speed: ") + dcenowplaying.qs_playbackSpeed
                    font.family: "Droid Sans"
                    font.pixelSize: 12

                }
            }
                    Text {
                        id: artist
                        text: qsTr("Artist :") + dcenowplaying.qs_mainTitle
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        font.bold: true
                        smooth: true
                        font.pixelSize: 12
                    }


                    Text {
                        id: album
                         wrapMode: "WrapAtWordBoundaryOrAnywhere"
                         text: qsTr("Album: ") + dcenowplaying.qs_mainTitle2
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true                      
                        font.pixelSize: 12

                    }

                    Text {
                        id: title
                         wrapMode: "WrapAtWordBoundaryOrAnywhere"
                         text: qsTr("Title: ") + dcenowplaying.qs_subTitle
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true                      
                        font.pixelSize: 12                      
                    }
                    Image {
                        id: nowplayingimage
                        width: scaleX(50)
                        height: scaleY(50)
                        fillMode: Image.PreserveAspectFit
                        source: "image://updateobject/"+dcenowplaying.m_iplaylistPosition
                    }
                }



            Remote_lighting_controls{ id: remote_lighting_controls1; }
            Remote_Audio_controls{ id: remote1; }
        }
    }

    HomeButton{anchors.right: parent.right; anchors.top:parent.top}

    VideoControls {
        id: videocontrols1
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

    }
}

