import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
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
    VideoControls {id: videocontrols1; anchors.top: mainrow.bottom; anchors.horizontalCenter: parent.horizontalCenter}

    Row{
        id:mainrow
        height: childrenRect.height
        width: childrenRect.width
        spacing: 5
        anchors.top:parent.top
        anchors.topMargin: scaleY(5)
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
                    font.family: "Droid Sans"
                    font.pixelSize: 12
                    color: style.highlight1
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
                        font.pixelSize: 12
                    }


                    Text {
                        id: album
                        wrapMode: "NoWrap"
                        text: qsTr("Channel: ") + dcenowplaying.channel
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 12
                    }

                    Text {
                        id: title
                        wrapMode: "NoWrap"
                        text: qsTr("Program:") + dcenowplaying.program
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 12
                    }
                }
            }
        }

RemoteNumberPad {id: remotenumberpad1;}
        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }

    }

}
