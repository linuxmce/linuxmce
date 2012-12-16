import QtQuick 1.0

import "../components"
import "../js/ComponentLoader.js" as MyJs


Rectangle {

    // property alias synText:
    id: storedvideoremote
    height: appH
    width: appW
    radius: 0
    opacity: 1
    color: "transparent"
    z:5
    Column{
        anchors.right: storedvideoremote.right
        anchors.rightMargin: scaleX(1)
        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }
    }
    Connections{
        target:dcenowplaying
        onImageChanged: {
            nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
            console.log("now playing changed")
        }
    }


    Row{
        id:mainrow
        height: scaleY(65)
        width: scaleX(95)
        spacing: scaleX(1)
        anchors.top:parent.top
        anchors.topMargin: scaleY(1.5)
        anchors.horizontalCenter: parent.horizontalCenter

        NonEPGPlaylist{
            id:playlist
            anchors.left: parent.left
            anchors.leftMargin: scaleX(1)
        }

        Column
        {
            id:metadata
            height: scaleY(50)
            width: 480
            spacing: scaleY(1)

            Rectangle {
                id: gradientheader
                width:metadata.width
                height: childrenRect.height
                color:"transparent"

                Image {
                    id: headerimage
                    source: "../img/icons/header.png"
                    height:parent.height
                    width:parent.width
                    opacity: .75
                }

                Text {
                    id: headertext
                    height:scaleY(2)
                    text:qsTr("Speed: ") + dceTimecode.playbackSpeed
                    font.family: "Droid Sans"
                    font.pixelSize: scaleY(2)
                    color: "aliceblue"
                }

                Text {
                    id: timecode
                    height:scaleY(2)
                    text: dceTimecode.qsCurrentTime + qsTr(" of ") + dceTimecode.qsTotalTime
                    font.family: "Droid Sans"
                    font.pixelSize: scaleY(1) *2.15
                    anchors.bottom:parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.bold: true
                    color:"aliceblue"
                }
            }

            Image {
                id: nowplayingimage
                anchors.top:gradientheader.bottom
                anchors.topMargin: scaleY(.5)
                width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(20)
                height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(50)
                source: ""
                Component.onCompleted: manager.setBoundStatus(true)
            }
        }

        ProgramData {
            id: textrect
        }
    }


    MediaScrollBar{id:media_transit; anchors.bottom: controlrow.top; anchors.horizontalCenter: controlrow.horizontalCenter}

    Row{
        id:controlrow
        anchors.top: mainrow.bottom
        anchors.topMargin: scaleY(2)
        height: childrenRect.height
        width: childrenRect.width
        anchors.horizontalCenter: parent.horizontalCenter
        z:0
        Column{
            height: childrenRect.height
            width: childrenRect.width
            spacing: scaleY(1.5)

            VideoControls {
                id: videocontrols1
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Row{
                height: childrenRect.height
                width: childrenRect.width
                spacing: scaleX(1)
                AvOptionButton{
                    buttontext: qsTr("Zoom & Aspect")
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  {
                            MyJs.createStageComponent("../components/ZoomAspect.qml", storedvideoremote)
                        }
                    }
                }
                AvOptionButton{
                    buttontext: qsTr("Bookmarks")
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  {

                            manager.ShowBookMarks()
                            MyJs.createStageComponent("../components/Bookmarks.qml", storedvideoremote)

                        }
                    }
                }
                AvOptionButton{
                    buttontext: qsTr("Resend AV Codes")
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  {
                            MyJs.createStageComponent("../components/Avcodes.qml", storedvideoremote)
                        }
                    }
                }
                AvOptionButton{
                    buttontext: qsTr("Manage Playlist")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: playlist.optionVisible ? playlist.optionVisible =false: playlist.optionVisible=true
                    }
                }
                AvOptionButton{
                    buttontext: qsTr("Thumbnail")
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  {
                            manager.grabScreenshot(dcenowplaying.filepath)
                            MyJs.createStageComponent("../components/AssignScreenShot.qml", storedvideoremote)

                        }
                    }
                }
                AvOptionButton{
                    buttontext: qsTr("Jog")
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  {
                            MyJs.createStageComponent("../components/JogPanel.qml", storedvideoremote)
                        }
                    }
                }
                AvOptionButton{
                    buttontext: qsTr("Power")
                }
                HomeButton{}

            }
        }

    }

}


