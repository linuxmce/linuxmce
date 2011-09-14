import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: dvdremote
    anchors.centerIn: parent
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
        spacing: 5
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter

        NonEPGPlaylist{ id:playlist}
        Rectangle {

            width: scaleX(30)
            height: scaleY(50)
            color: style.advanced_bg

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

            Image {
                id: nowplayingimage
                width: scaleX(30)
                height: scaleY(30)
                anchors.top: gradientheader.bottom
                fillMode: Image.PreserveAspectFit
                source: "image://updateobject/"+dcenowplaying.m_iplaylistPosition
            }

            Rectangle {
                id: metadatavideo
                width: scaleX(30)
                height: childrenRect.height
                color: style.darkhighlight
                anchors.top:nowplayingimage.bottom
                Column
                {
                    width: scaleX(30)
                    spacing: 5
                    height: metadatavideo.height

                    Text {
                        id: maintitle
                        width: parent.width
                        text: qsTr("Title: ") + dcenowplaying.mediatitle
                        font.family: "Droid Sans"
                        wrapMode:"WrapAtWordBoundaryOrAnywhere"
                        font.bold: true
                        smooth: true
                        font.pixelSize: 12
                    }


                    Text {
                        id: subtitle
                        width: parent.width
                        wrapMode:"WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Starring: ") + dcenowplaying.performerlist
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        font.pixelSize: 12
                    }

                    Text {
                        id: director
                        width: parent.width
                        wrapMode:"WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Director: ") + dcenowplaying.director
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        font.pixelSize: 12
                    }

                    Text {
                        id: genre
                        width: parent.width
                         wrapMode:"WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Genre: ") + dcenowplaying.genre
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        font.pixelSize: 12
                    }
                }
            }
        }
        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }

    }

        HomeButton{anchors.right: parent.right; anchors.top:parent.top}

        VideoControls {
            id: videocontrols1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom

        }




    }

