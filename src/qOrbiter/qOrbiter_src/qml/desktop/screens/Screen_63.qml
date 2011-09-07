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
    radius: 0
    opacity: 1
    color: "transparent"
    Component.onCompleted:setNowPlayingTv()
    //main 'now playing rect containing all the other items
    Remote_lighting_controls{ id: remote_lighting_controls1; x: 1045; y: 258; width: remote1.width; height: remote1.height }
    Remote_Audio_controls{ id: remote1; x: 1034; y: 38; anchors.rightMargin: 13; z: 45; }

    HomeButton{anchors.right: parent.right; anchors.top:parent.top}

    VideoControls {
        id: videocontrols1
        x: 309
        y: 598

    }

    EPGPlaylist{ x: 116;y: 38}




    Rectangle {
        x: 384
        y: 38
        width: 350
        height: 350
        color: style.advanced_bg
        anchors.verticalCenterOffset: -147
        anchors.horizontalCenterOffset: -81
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            id: rectangle1
            x: 0
            y: 0
            width: 350
            height: 38
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
                x: 0
                y: 12
                text:"Speed: " + dcenowplaying.qs_playbackSpeed
                font.family: "Droid Sans"
                font.pixelSize: 12
            }
        }

        Image {
            id: image1
            x: 0
            y: 38
            width: 350
            height: 224
            anchors.topMargin: 38
            anchors.bottomMargin: 88
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent

            source: "image://updateobject/"+dcenowplaying.m_iplaylistPosition
        }

        Rectangle {
            id: rectangle2
            x: 0
            y: 262
            width: 350
            height: 88
            color: "#ffffff"
            Column
            {
                width: 350
                spacing: 5
                height: rectangle2.height

                Text {
                    id: nowplayingboxtext
                    x: 0
                    y: 10
                    width: parent.width
                    height: 18
                    text: "Artist: " + dcenowplaying.qs_mainTitle
                    font.family: "Droid Sans"
                    anchors.topMargin: 10
                    anchors.top: parent.top
                    wrapMode: "NoWrap"
                    font.bold: true
                    smooth: true

                    font.pixelSize: 12
                }

                Text {
                    id: nowplayingboxtext2
                    x: 0
                    y: 35
                    width: parent.width
                    height: 18
                    text: "Album: " + dcenowplaying.qs_mainTitle2
                    font.family: "Droid Sans"
                    anchors.topMargin: 35
                    anchors.top: parent.top
                    wrapMode: Text.WrapAnywhere
                    font.bold: true
                    smooth: true

                    font.pixelSize: 12
                }

                Text {
                    id: titlebox
                    x: 0
                    y: 61

                    anchors.bottom: parent.bottom
                    wrapMode: "NoWrap"
                    text: "Device:" + dcenowplaying.qs_subTitle
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

    RemoteNumberPad {
        id: remotenumberpad1
        x: 734
        y: 38
    }
}
