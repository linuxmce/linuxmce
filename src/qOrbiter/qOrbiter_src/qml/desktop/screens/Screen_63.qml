import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
Rectangle {

    // property alias synText:
    id: satcableboxremote
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
    color: style.highlight2
    Component.onCompleted:setNowPlayingTv()

    Rectangle{
        height: scaleY(95)
        width: scaleX(95)
        color: style.darkhighlight
        radius: 10
        border.color: style.highlight1
        border.width: 2
        anchors.centerIn: parent


    //main 'now playing rect containing all the other items

    Row{
        id:mainrow
        height: childrenRect.height
        width: childrenRect.width
        spacing: 5
        anchors.top:parent.top
        anchors.topMargin: scaleY(2)
        anchors.horizontalCenter: parent.horizontalCenter
        EPGPlaylist{id:tvchannellist }

        Rectangle {
            id:metarect
            width: scaleX(35)
            height: childrenRect.height
            color: style.bgcolor

            Rectangle {
                id: gradientheader
                width: parent.width
                height: childrenRect.height
                Image {
                    id: headerimage
                    source: "../../../img/icons/header.png"
                    height:parent.height
                    width:parent.width
                }
                Text {
                    id: headertext
                    text:"Speed: " + dcenowplaying.qs_playbackSpeed
                    height: scaleY(2.5)
                    font.family: "Droid Sans"
                   font.pixelSize: 1 * scaleY(2)
                    color: "aliceblue"
                }
            }

            Image {
                id: nowplayingimage
                width: scaleX(35)
                height: scaleY(35)
                anchors.top: gradientheader.bottom
                fillMode: Image.PreserveAspectFit
                source: "image://updateobject/"+dcenowplaying.m_iplaylistPosition
            }

            Rectangle {
                id: metadatavideo
                width: scaleX(35)
                height: childrenRect.height

                Image {
                    id: npgloss
                    source: "../../../img/icons/header.png"
                    height:parent.height
                    width:parent.width
                    smooth:true
                    opacity: .35
                }

                color: style.bgcolor
                anchors.top:nowplayingimage.bottom
                Column
                {
                    width: scaleX(30)
                    spacing: 5
                    height: childrenRect.height

                    Text {
                        id: artist
                        width: parent.width
                        text: qsTr("Device: ")  + dcenowplaying.qs_mainTitle
                        font.family: "Droid Sans"
                        wrapMode: "NoWrap"
                        font.bold: true
                        smooth: true
                        font.pixelSize: 1 * scaleY(2)
                        color: "aliceblue"
                    }


                    Text {
                        id: album
                        wrapMode: "NoWrap"
                        text: qsTr("Channel: ") + dcenowplaying.channel
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        horizontalAlignment: Text.AlignHCenter
                       font.pixelSize: 1 * scaleY(2)
                        color: "aliceblue"
                    }

                    Text {
                        id: title
                        wrapMode: "NoWrap"
                        text: qsTr("Program:") + dcenowplaying.tvProgram
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        horizontalAlignment: Text.AlignHCenter
                         font.pixelSize: 1 * scaleY(2)
                        color: "aliceblue"
                    }
                }
            }
        }

RemoteNumberPad {id: remotenumberpad1;}
        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }
    }
    Row{
        id:controlrow
        anchors.top: mainrow.bottom
        anchors.topMargin: scaleY(2)
        height: childrenRect.height
        width: childrenRect.width
        anchors.horizontalCenter: parent.horizontalCenter
        Column{
            height: childrenRect.height
            width: childrenRect.width

            VideoControls {
                id: videocontrols1
            }
                Row{
                    height: childrenRect.height
                    width: childrenRect.width
                    spacing: scaleX(1)
                    ButtonSq{
                        buttontext: qsTr("Zoom & Aspect")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createAvComponent("../components/ZoomAspect.qml",satcableboxremote )
                            }
                        }
                    }
                    ButtonSq{
                        buttontext: qsTr("PVR Menu")
                    }
                    ButtonSq{
                        buttontext: qsTr("Resend AV Codes")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createAvComponent("../components/Avcodes.qml",satcableboxremote )
                            }
                        }
                    }
                    ButtonSq{
                        buttontext: qsTr("Manage Playlist")
                    }
                    ButtonSq{
                        buttontext: qsTr("Power")
                    }
                    HomeButton{}
                }
            }
        }
    }
}
