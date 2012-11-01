import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
Rectangle {

    // property alias synText:
    id: storedvideoremote
    height: style.orbiterH
    width: style.orbiterW
    radius: 0
    opacity: 1
    color: "transparent"

    Column{
        anchors.right: storedvideoremote.right
        anchors.rightMargin: scaleX(1)
        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }
    }

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
        onPlayListPositionChanged: { nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;}
    }

    Component.onCompleted:setNowPlayingData()

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
                width: dcenowplaying.aspect=="wide"? nowplayingimage.width : childrenRect.width
                spacing: scaleY(1)

                Rectangle {
                    id: gradientheader
                    width:metadata.width
                    height: childrenRect.height
                    color:"transparent"

                    Image {
                        id: headerimage
                        source: "../images/header.png"
                        height:parent.height
                        width:parent.width
                        opacity: .75
                    }

                    Text {
                        id: headertext
                        height:scaleY(2)
                        text:qsTr("Speed: ") + dcenowplaying.qs_playbackSpeed
                        font.family: "Droid Sans"
                        font.pixelSize: scaleY(2)
                        color: "aliceblue"
                    }

                    Text {
                        id: timecode
                        height:scaleY(2)
                        text: dcenowplaying.timecode + qsTr(" of ") + dcenowplaying.duration
                        font.family: "Droid Sans"
                        font.pixelSize: scaleY(1) *2.15
                        anchors.bottom:parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.bold: true
                        color:"aliceblue"
                    }
                }

                Rectangle{
                    id:imageholder
                    height:childrenRect.height
                    width:childrenRect.width
                    color: "transparent"
                    anchors.top:gradientheader.bottom
                    anchors.topMargin: scaleY(.5)

                    BorderImage {
                        id: borderimg
                        horizontalTileMode: BorderImage.Repeat
                        source: "../images/drpshadow.png"
                        anchors.fill: nowplayingimage
                        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                        border { left: 10; top: 10; right: 10; bottom: 10 }
                        smooth: true
                    }
                    Image {
                        id: nowplayingimage
                        width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(20)
                        height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(50)
                        source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
                    }
                }



            }

            Rectangle{
                id:textrect
                height: childrenRect.height
                width: dcenowplaying.aspect=="wide"?  textcol.width : scaleX(20)
                color:"transparent"
                anchors.left:  imageholder.right;
                anchors.top:   imageholder.top
                /*
            BorderImage {
                id: metaborder
                horizontalTileMode: BorderImage.Repeat
                source: "../images/drpshadow.png"
                anchors.fill: textrect
                anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                border { left: 10; top: 10; right: 10; bottom: 10 }
                smooth: true
            }
*/
                z:10
                Column{
                    id:textcol
                    spacing: scaleY(1.5)
                    width: nowplayingimage.width
                    height: childrenRect.height


                    Text {
                        id: generaltitle
                        width: parent.width
                        text:  dcenowplaying.qs_mainTitle
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        font.bold: true
                        smooth: true
                        font.pixelSize: scaleY(3)
                        visible:  dcenowplaying.qs_mainTitle =="" ? false: true
                    }

                    Text {
                        id: programtext
                        width: parent.width
                        text: qsTr("Program :") + dcenowplaying.tvProgram
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        //  font.bold: true
                        smooth: true
                        font.pixelSize: scaleY(1.5)
                        visible:  dcenowplaying.tvProgram =="" ? false: true
                    }

                    Text {
                        id: episode
                        width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Episode: ") + dcenowplaying.episode
                        font.family: "Droid Sans"
                        //  font.bold: true
                        smooth: true
                        font.pixelSize: scaleY(1.5)
                        visible:  dcenowplaying.episode =="" ? false: true
                    }

                    Text {
                        id: genre
                        width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Genre(s): ") + dcenowplaying.genre
                        font.family: "Droid Sans"
                        // font.bold: true
                        smooth: true
                        font.pixelSize: scaleY(1.5)
                        visible:  dcenowplaying.genre =="" ? false: true
                        MouseArea{
                            anchors.fill: genre
                            hoverEnabled: true
                            onEntered: { genre.elide = "ElideNone" ; }
                            onExited: {genre.elide = "ElideRight"; }
                        }
                    }


                    Text {
                        id: starring
                        width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Perfomers: ") + dcenowplaying.performerlist
                        font.family: "Droid Sans"
                        //  font.bold: true
                        smooth: true
                        font.pixelSize: scaleY(2)
                        elide: "ElideRight"
                        visible:  dcenowplaying.performerlist =="" ? false: true

                        MouseArea{
                            anchors.fill: starring
                            hoverEnabled: true
                            onEntered: { starring.elide = "ElideNone" ; }
                            onExited: {starring.elide = "ElideRight"; }
                        }
                    }

                    Text {
                        id: synopsis
                        width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Synopsis: ") + dcenowplaying.synop
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        font.pixelSize: scaleY(1.5)
                        elide: "ElideRight"
                        visible:  dcenowplaying.synop =="" ? false: true

                        MouseArea{
                            anchors.fill: synopsis
                            hoverEnabled: true
                            onEntered: { synopsis.elide = "ElideNone" ; }
                            onExited: {synopsis.elide = "ElideRight"; }
                        }
                    }
                }
            }
        }

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
                                    MyJs.createAvComponent("../components/ZoomAspect.qml", storedvideoremote)
                                }
                            }
                        }
                        AvOptionButton{
                            buttontext: qsTr("Bookmarks")
                            MouseArea{
                                anchors.fill: parent
                                onClicked:  {

                                    MyJs.createThumbComponent("../components/Bookmarks.qml", storedvideoremote)

                                }
                            }
                        }
                        AvOptionButton{
                            buttontext: qsTr("Resend AV Codes")
                            MouseArea{
                                anchors.fill: parent
                                onClicked:  {
                                    MyJs.createAvComponent("../components/Avcodes.qml", storedvideoremote)
                                }
                            }
                        }
                        AvOptionButton{
                            buttontext: qsTr("Manage Playlist")
                        }
                        AvOptionButton{
                            buttontext: qsTr("Thumbnail")
                            MouseArea{
                                anchors.fill: parent
                                onClicked:  {
                                    manager.grabScreenshot()
                                    MyJs.createThumbComponent("../components/AssignScreenShot.qml", storedvideoremote)

                                }
                            }
                        }
                        AvOptionButton{
                            buttontext: qsTr("Jog")
                            MouseArea{
                                anchors.fill: parent
                                onClicked:  {
                                    MyJs.createAvComponent("../components/JogPanel.qml", storedvideoremote)
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


