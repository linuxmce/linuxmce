import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs
Rectangle {

    // property alias synText:
    id: satcableboxremote
    height: appH
    width: appW
    color:"transparent"


    Connections{
        target:dcenowplaying
        onPlayListPositionChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Component.onCompleted:setNowPlayingTv()

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
                        source: "../img/icons/header.png"
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
                    source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
                }

                Rectangle {
                    id: metadatavideo
                    width: scaleX(35)
                    height: childrenRect.height

                    Image {
                        id: npgloss
                        source: "../img/icons/header.png"
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
                            id: np_device
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
                            id: network_id
                            wrapMode: "NoWrap"
                            text: qsTr("Network: ") + dcenowplaying.channelID
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: scaleY(2)
                            color: "white"
                        }


                        Text {
                            id: channel_id
                            wrapMode: "NoWrap"
                            text: qsTr("Channel: ") + dcenowplaying.channel
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: scaleY(2)
                            color: "white"
                        }

                        Text {
                            id: program_title
                            wrapMode: "NoWrap"
                            text: qsTr("Program:") + dcenowplaying.tvProgram
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: scaleY(2)
                            color: "white"
                        }

                        Text {
                            id: live_av
                            wrapMode: "NoWrap"
                            text: qsTr("ScreenShot Availible:") + manager.monitorAvailible
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: scaleY(2)
                            color: "white"
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
            width: mainrow.width
            anchors.horizontalCenter: parent.horizontalCenter

            Column{
                height: childrenRect.height
                width: parent.width

                spacing: scaleY(1)
                VideoControls {
                    id: videocontrols1
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Row{
                    spacing:scaleY(1)
                    anchors.horizontalCenter: parent.horizontalCenter
                    AvOptionButton{
                        id:zoom
                        buttontext: qsTr("Zoom & Aspect")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createStageComponent("../components/ZoomAspect.qml",satcableboxremote )
                            }
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("PVR Menu")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: manager.extraButtons("pvrmenu")
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Resend AV Codes")
                        MouseArea{
                            anchors.fill: parent
                            onClicked:  {
                                MyJs.createStageComponent("../components/Avcodes.qml",satcableboxremote )
                            }
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Thumbnail")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: manager.grabScreenshot()
                        }
                    }
                    Flickable{
                        height: zoom.height
                        width: scaleX(35)
                        flickableDirection: "HorizontalFlick"
                        clip:true
                        boundsBehavior: "DragAndOvershootBounds"

                        contentWidth: scaleX(100)
                        Row{
                            width:scaleX(100)
                            height: parent.height

                            AvOptionButton{
                                buttontext: qsTr("Live")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("livetv")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Schedule")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("schedule")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Recordings")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("recordings")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Music")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("music")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Guide")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("guide")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Menu")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("menu")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Info")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("info")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Exit")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("exit")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Favorites")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("favorites")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Record")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("record")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Help")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("help")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Bookmark Channel")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("channelbookmark")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Change Inputs")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("changeinputs")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Page Up")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("pageup")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Page Down")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("pagedown")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Next Day")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("nextday")
                                }
                            }
                            AvOptionButton{
                                buttontext: qsTr("Previous day")
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: manager.extraButtons("previousday")
                                }
                            }
                        }

                    }
                    AvOptionButton{
                        buttontext: qsTr("Power")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: manager.stopMedia()
                        }
                    }
                    HomeButton{}
                }
            }
        }

}
