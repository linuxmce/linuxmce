import QtQuick 2.0
import "../components"

Rectangle {

    // property alias synStyledText:
    id: satcableboxremote
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
	Column {
                id: epgandColouredButtons
                EPGPlaylist{
                        id:tvchannellist
                }

                Rectangle {
                        height: scaleY(5)
                        color: "#00000000"
                        width: tvchannellist.width
                        anchors.margins: scaleY(5)

                        Rectangle {
                            color: "#ff0000"
                            x: parent.width / 40
                            id: redbutton
                            width: parent.width / 5
                            height: parent.height
                            MouseArea{
                                onClicked: manager.redButton()
                            }
                        }
                        Rectangle {
                            anchors.left: redbutton.right
                            anchors.leftMargin: parent.width / 20
                            color: "#00ff00"
                            id: greenbutton
                            width: parent.width / 5
                            height: parent.height
                            MouseArea{
                                onClicked: manager.greenButton()
                            }
                        }
                        Rectangle {
                            anchors.left: greenbutton.right
                            anchors.leftMargin: parent.width / 20
                            color: "#ffff00"
                            id: yellowbutton
                            width: parent.width / 5
                            height: parent.height
                            MouseArea{
                                onClicked: manager.yellowButton()
                            }
                        }
                        Rectangle {
                            anchors.left: yellowbutton.right
                            anchors.leftMargin: parent.width / 20
                            color: "#0000ff"
                            id: bluebutton
                            width: parent.width / 5
                            height: parent.height
                            MouseArea{
                                onClicked: manager.blueButton()
                            }
                        }
                }
        }
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
                color: "#00000000" // just let the parent shine through.
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
                        font.pixelSize: 12
                    }


                    StyledText {
                        id: album
                        wrapMode: "NoWrap"
                        text: qsTr("Channel: ") + dcenowplaying.qs_mainTitle2
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        horizontalAlignment: StyledText.AlignHCenter
                        font.pixelSize: 12
                    }

                    StyledText {
                        id: title
                        wrapMode: "NoWrap"
                        text: qsTr("Program:") + dcenowplaying.program
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        horizontalAlignment: StyledText.AlignHCenter
                        font.pixelSize: 12
                    }
                }
            }
        }


        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }

    }

}
