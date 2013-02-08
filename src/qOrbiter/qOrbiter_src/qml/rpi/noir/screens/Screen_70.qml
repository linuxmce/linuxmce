import QtQuick 2.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
Rectangle {

    // property alias synStyledText:
    id: storedvideoremote
    height: manager.appHeight
    width: manager.appWidth
    radius: 0
    opacity: 1
    color: "transparent"


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
                width: dcenowplaying.aspect=="wide"? nowplayingimage.width : childrenRect.width
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

                    StyledText {
                        id: headertext
                        height:scaleY(2)
                        text:qsTr("Speed: ") + dceTimecode.playbackSpeed
                        font.family: "Droid Sans"
                        font.pointSize: scaleY(2)
                        color: "aliceblue"
                    }

                    StyledText {
                        id: timecode
                        height:scaleY(2)
                        text: dceTimecode.qsCurrentTime + qsTr(" of ") + dceTimecode.qsTotalTime
                        font.family: "Droid Sans"
                        font.pointSize: scaleY(1) *2.15
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
                        source: "../img/icons/drpshadow.png"
                        anchors.fill: nowplayingimage
                        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                        border { left: 10; top: 10; right: 10; bottom: 10 }
                        smooth: true
                    }
                    Image {
                        id: nowplayingimage
                        width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(20)
                        height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(50)
                        source: ""
                        Component.onCompleted: manager.setBoundStatus(true)
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
                source: "../../../img/icons/drpshadow.png"
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


                    StyledText {
                        id: generaltitle
                        width: parent.width
                        text:  dcenowplaying.qs_mainTitle
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        font.bold: true
                        smooth: true
                        font.pointSize: scaleY(3)
                        visible:  dcenowplaying.qs_mainTitle =="" ? false: true
                    }

                    StyledText {
                        id: programtext
                        width: parent.width
                        text: qsTr("Program :") + dcenowplaying.tvProgram
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        //  font.bold: true
                        smooth: true
                        font.pointSize: scaleY(1.5)
                        visible:  dcenowplaying.tvProgram =="" ? false: true
                    }

                    StyledText {
                        id: episode
                        width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Episode: ") + dcenowplaying.episode
                        font.family: "Droid Sans"
                        //  font.bold: true
                        smooth: true
                        font.pointSize: scaleY(1.5)
                        visible:  dcenowplaying.episode =="" ? false: true
                    }

                    StyledText {
                        id: genre
                        width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Genre(s): ") + dcenowplaying.genre
                        font.family: "Droid Sans"
                        // font.bold: true
                        smooth: true
                        font.pointSize: scaleY(1.5)
                        visible:  dcenowplaying.genre =="" ? false: true
                        MouseArea{
                            anchors.fill: genre
                            hoverEnabled: true
                            onEntered: { genre.elide = "ElideNone" ; }
                            onExited: {genre.elide = "ElideRight"; }
                        }
                    }


                    StyledText {
                        id: starring
                        width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Perfomers: ") + dcenowplaying.performerlist
                        font.family: "Droid Sans"
                        //  font.bold: true
                        smooth: true
                        font.pointSize: scaleY(2)
                        elide: "ElideRight"
                        visible:  dcenowplaying.performerlist =="" ? false: true

                        MouseArea{
                            anchors.fill: starring
                            hoverEnabled: true
                            onEntered: { starring.elide = "ElideNone" ; }
                            onExited: {starring.elide = "ElideRight"; }
                        }
                    }

                    StyledText {
                        id: synopsis
                        width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: qsTr("Synopsis: ") + dcenowplaying.synop
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        font.pointSize: scaleY(1.5)
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


