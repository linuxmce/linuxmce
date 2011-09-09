import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: cablesatboxremote
    anchors.centerIn: parent

    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true

        onTriggered: image1.source = "image://updateobject/"+securityvideo.timestamp
    }

    Connections{
        target:dcenowplaying
        onPlayListPositionChanged: image1.source = "image://updateobject/"+securityvideo.timestamp
    }

    height: 720
    width: 1280
    color: "transparent"
    Component.onCompleted:setNowPlayingTv()
    //main 'now playing rect containing all the other items


    HomeButton{anchors.right: parent.right; anchors.top:parent.top}

    VideoControls {
        id: videocontrols1
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row
    {
        height: scaleY(50)
        width:scaleX(90)

        EPGPlaylist{ }
        RemoteNumberPad {
            id: remotenumberpad1

        }
        Rectangle {
            id:nowplayingbox
            width: scaleX(30)
            height: scaleY(40)
            color: style.advanced_bg

            Rectangle {
                id: headergradient
                anchors.top: nowplayingbox.top
                width: scaleX(30)
                height: scaleY(5)
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
                    id: text1
                    anchors.centerIn: parent
                    text:"Speed: " + dcenowplaying.qs_playbackSpeed
                    font.family: "Droid Sans"
                    font.pixelSize: 12
                }
            }

            Image {
                id: image1
                anchors.top: headergradient.bottom
                width: scaleX(40)
                height: scaleY(40)
                fillMode: Image.PreserveAspectFit
                source: "image://updateobject/"+dcenowplaying.m_iplaylistPosition
            }

            Rectangle {
                id: metadatarect
                width: parent.width
                height: scaleY(9)
                color: style.lighthighlight
                anchors.bottom: image1.bottom
                Column
                {
                    width: scaleX(30)
                    spacing: 5
                    height: metadatarect.height

                    Text {
                        id: titlebox
                        text: "Device: " + dcenowplaying.qs_mainTitle
                        wrapMode: "NoWrap"
                        font.family: "Droid Sans"

                        font.bold: true
                        smooth: true
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 12
                    }
                    Text {
                        id: progbox
                        text: "Program: "
                        wrapMode: "NoWrap"
                        font.family: "Droid Sans"
                        anchors.bottomMargin: 12
                        font.bold: true
                        smooth: true
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 12
                    }
                }
            }
        }
     Remote_lighting_controls{ id: remote_lighting_controls1; width: remote1.width; height: remote1.height }
        Remote_Audio_controls{ id: remote1; anchors.rightMargin: 13; z: 45; }
    }
}
