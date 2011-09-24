import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
Rectangle {

    // property alias synText:
    id: storedvideoremote

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

    height: style.orbiterH
    width: style.orbiterW
    radius: 0
    opacity: 1
    color: style.darkhighlight
    Component.onCompleted:setNowPlayingData()


    //main 'now playing rect containing all the other items
    Image {
        id: panelimg
        source: "../../../img/icons/displaypanel.png"
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
                    width: childrenRect.width

                    Rectangle {
                        id: gradientheader
                        width:scaleX(35)
                        height: childrenRect.height
                        color:"transparent"

                        Image {
                            id: headerimage
                            source: "../../../img/icons/header.png"
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
                        BorderImage {
                            id: borderimg
                            horizontalTileMode: BorderImage.Repeat
                            source: "../../../img/icons/drpshadow.png"
                            anchors.fill: nowplayingimage
                            anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                            border { left: 10; top: 10; right: 10; bottom: 10 }
                            smooth: true
                        }
                        Image {
                            id: nowplayingimage
                            width: dcenowplaying.aspect=="wide"? scaleX(25) : scaleX(15)
                            height:dcenowplaying.aspect=="wide"? scaleY(25) : scaleY(45)
                            source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition

                        }
                    }

                    Rectangle{
                        id:textrect
                        height: childrenRect.height
                        width: childrenRect.width
                        color:"transparent"
                        anchors.left: dcenowplaying.aspect == "wide"? imageholder.left : imageholder.right;
                        anchors.verticalCenter:  dcenowplaying.aspect == "wide"? imageholder.bottom : imageholder.verticalCenter

                        Column{
                            spacing: scaleY(1.5)


                            Text {
                                id: generaltitle
                                width: scaleX(35)
                                text: qsTr("Ttitle :") + dcenowplaying.qs_mainTitle
                                font.family: "Droid Sans"
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                font.bold: true
                                smooth: true
                                font.pixelSize: scaleY(1.5)
                                visible:  dcenowplaying.qs_mainTitle =="" ? false: true
                            }

                            Text {
                                id: programtext
                                width: scaleX(35)
                                text: qsTr("Program :") + dcenowplaying.tvProgram
                                font.family: "Droid Sans"
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                font.bold: true
                                smooth: true
                                font.pixelSize: scaleY(1.5)
                                visible:  dcenowplaying.tvProgram =="" ? false: true
                            }

                            Text {
                                id: episode
                                width: scaleX(35)
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                text: qsTr("Episode: ") + dcenowplaying.episode
                                font.family: "Droid Sans"
                                font.bold: true
                                smooth: true
                                font.pixelSize: scaleY(1.5)
                                visible:  dcenowplaying.episode =="" ? false: true
                            }

                            Text {
                                id: genre
                                width: scaleX(35)
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                text: qsTr("Genre(s): ") + dcenowplaying.genre
                                font.family: "Droid Sans"
                                font.bold: true
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
                                width: scaleX(35)
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                text: qsTr("Starring: ") + dcenowplaying.performerlist
                                font.family: "Droid Sans"
                                font.bold: true
                                smooth: true
                                font.pixelSize: scaleY(1.5)
                                elide: "ElideRight"
                                visible:  dcenowplaying.performerlist =="" ? false: true

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

                VideoControls {
                    id: videocontrols1
                    anchors.horizontalCenter: parent.horizontalCenter
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
                                MyJs.createAvComponent("../components/ZoomAspect.qml", storedvideoremote)
                            }
                        }
                    }
                    ButtonSq{
                        buttontext: qsTr("Bookmarks")
                    }
                    ButtonSq{
                        buttontext: qsTr("Resend AV Codes")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createAvComponent("../components/Avcodes.qml", storedvideoremote)
                            }
                        }
                    }
                    ButtonSq{
                        buttontext: qsTr("Manage Playlist")
                    }
                    ButtonSq{
                        buttontext: qsTr("Thumbnail")
                    }
                    ButtonSq{
                        buttontext: qsTr("Jog")
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

