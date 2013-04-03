import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs
Rectangle {

    // property alias synText:
    id: dvd_remote
    height: manager.appHeight
    width: manager.appWidth
    radius: 0
    opacity: 1
    color: "transparent"


    Connections{
        target:dcenowplaying
        onImageChanged:nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
        onPlayListPositionChanged: { nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;}
    }

    Component.onCompleted:setNowPlayingData()


    //main 'now playing rect containing all the other items
    Image {
        id: panelimg
        source: "../img/icons/displaypanel.png"
        height: containerrect.height
        width:containerrect.width
        anchors.centerIn: containerrect
    }

    Rectangle{
        id:containerrect
        height:childrenRect.height + scaleY(5)
        width: childrenRect.width + scaleX(4)
        clip:true
        radius: 10
        border.color: style.highlight1
        border.width: 3
        color: "transparent"
        anchors.centerIn: parent

        Row{
            id:mainrow
            height: childrenRect.height
            width: childrenRect.width
            spacing: scaleX(1)
            anchors.top:parent.top
            anchors.topMargin: scaleY(1.5)
            anchors.horizontalCenter: parent.horizontalCenter

            NonEPGPlaylist{ id:playlist}

            Row {
                id:metarow
                width: childrenRect.width
                height: childrenRect.height
                spacing: scaleX(1)

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
                            source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
                        }
                    }


                    BorderImage {
                        id: metaborder
                        horizontalTileMode: BorderImage.Repeat
                        source: "../img/icons/drpshadow.png"
                        anchors.fill: textrect
                        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                        border { left: 10; top: 10; right: 10; bottom: 10 }
                        smooth: true
                    }

                    Rectangle{
                        id:textrect
                        height: childrenRect.height
                        width: dcenowplaying.aspect=="wide"?  textcol.width : scaleX(20)
                        color:style.highlight1
                        anchors.left: dcenowplaying.aspect == "wide"? imageholder.left : imageholder.right;
                        anchors.top:  dcenowplaying.aspect == "wide"? imageholder.bottom: imageholder.top
                        anchors.bottomMargin: dcenowplaying.aspect == "wide"? 20 : 20
                        Column{
                            id:textcol
                            spacing: scaleY(1.5)
                           width: nowplayingimage.width
                           height: childrenRect.height


                            Text {
                                id: generaltitle
                                width: parent.width
                                text: qsTr("Title: ") + dcenowplaying.qs_mainTitle
                                font.family: "Droid Sans"
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                font.bold: true
                                smooth: true
                                font.pixelSize: scaleY(1.75)
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
                                    anchors.fill: starring
                                    hoverEnabled: true
                                    onEntered: { starring.elide = "ElideNone" ; }
                                    onExited: {starring.elide = "ElideRight"; }
                                }
                            }

                        }

                    }

                }
                Remote_lighting_controls{ id: remote_lighting_controls1; }
                Remote_Audio_controls{ id: remote1; }
            }
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
                                MyJs.createStageComponent("../components/ZoomAspect.qml", dvd_remote)
                            }
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Bookmarks")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Resend AV Codes")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createStageComponent("../components/Avcodes.qml", dvd_remote)
                            }
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Menu")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: manager.dvd_showMenu(true)
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Jog")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createStageComponent("../components/JogPanel.qml", dvd_remote)
                            }
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Dvd Options")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Copy Disc")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Bookmarks")
                    }
                    AvOptionButton{
                        buttontext: qsTr("View Attributes")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Thumbnail")
                    }

                    AvOptionButton{
                        buttontext: qsTr("Power")
                    }
                    HomeButton{}
                }
            }
        }
    }
}

