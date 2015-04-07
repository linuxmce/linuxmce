import QtQuick 2.0
import "../components"

Rectangle {

    // property alias synStyledText:
    id: mythtvremote
    anchors.centerIn: parent

    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true

        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Connections{
        target:dcenowplaying
        onPlayListPositionChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }
    height: manager.appHeight
    width: manager.appWidth
    radius: 0
    opacity: 1
    color: style.darkhighlight
    Component.onCompleted:setNowPlayingTv()

    //main 'now playing rect containing all the other items
    HomeButton{anchors.right: parent.right; anchors.top:parent.top}
    VideoControls {id: videocontrols1; anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter}

    Row{
        id:mainrow
        height: childrenRect.height
        width: childrenRect.width
        spacing: 5
        anchors.top:parent.top
        anchors.topMargin: scaleY(10)
        anchors.horizontalCenter: parent.horizontalCenter
        EPGPlaylist{ }
        RemoteNumberPad {id: remotenumberpad1;}
        Rectangle {
            width: scaleX(30)
            height: childrenRect.height
            color: style.accentcolor

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
                StyledText {
                    id: headertext
                    text:"Speed: " + dcenowplaying.qs_playbackSpeed
                    font.family: "Droid Sans"
                    font.pointSize: 12
                }
            }

            Image {
                id: nowplayingimage
                width: scaleX(30)
                height: scaleY(30)
                anchors.top: gradientheader.bottom
                fillMode: Image.PreserveAspectFit
                source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
            }

            Rectangle {
                id: metadatavideo
                width: scaleX(30)
                height: childrenRect.height

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: style.accentcolor
                    }

                    GradientStop {
                        position: 1
                        color: style.darkhighlight
                    }
                }
                color: style.darkhiglight
                anchors.top:nowplayingimage.bottom
                Column
                {
                    width: scaleX(30)
                    spacing: 5
                    height: childrenRect.height

                    StyledText {
                        id: artist
                        width: parent.width
                        text: qsTr("Device: ")  + dcenowplaying.qs_mainTitle
                        font.family: "Droid Sans"
                        wrapMode: "NoWrap"
                        font.bold: true
                        smooth: true
                        font.pointSize: 12
                    }


                    StyledText {
                        id: album
                        wrapMode: "NoWrap"
                        text: qsTr("Channel: ") + dcenowplaying.qs_mainTitle2
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        horizontalAlignment: StyledText.AlignHCenter
                        font.pointSize: 12
                    }

                    StyledText {
                        id: title
                        wrapMode: "NoWrap"
                        text: qsTr("Program:") + dcenowplaying.program
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        horizontalAlignment: StyledText.AlignHCenter
                        font.pointSize: 12
                    }
                }
            }
        }


        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }

    }

}
