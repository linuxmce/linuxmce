import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: storedaudioremote

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

    height: style.orbiterH
    width: style.orbiterW
    radius: 0
    opacity: 1
    color: style.darkhighlight
    Component.onCompleted:setNowPlayingData()

Rectangle{

        height:childrenRect.height + scaleY(2)
        width: childrenRect.width + scaleX(2)
        clip:true
        radius: 10
        border.color: style.highlight1
        border.width: 5
        color: style.lighthighlight
        anchors.centerIn: parent



    //main 'now playing rect containing all the other items

        Row{
            id:headerrow
            height:scaleY(10)
            width: parent.width-1
            Image {
                id: headerimage
                source: "../../../img/icons/header.png"
                height:scaleY(10)
                width:parent.width
            }

            Text {
                id: headertext
                height:scaleY(5)
                text:qsTr("Speed: ") + dcenowplaying.qs_playbackSpeed
                font.family: "Droid Sans"
                font.pixelSize: scaleY(2)
                color: "aliceblue"
            }

            Text {
                id: timecode
                height:scaleY(5)
                text: dcenowplaying.timecode + qsTr(" of ") + dcenowplaying.duration
                font.family: "Droid Sans"
                font.pixelSize: scaleY(2.15)
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold: true
                color:"aliceblue"
            }

            Text {
                id: hartist
                text: dcenowplaying.qs_mainTitle + " - " + dcenowplaying.qs_subTitle + qsTr(" | From the album: ")  + dcenowplaying.qs_mainTitle2
                font.family: "Droid Sans"
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.bold: true
                smooth: true
               font.pixelSize: scaleY(2.25)
              anchors.bottom: parent.bottom
              anchors.bottomMargin: scaleY(1.5)
               anchors.left: parent.left
               anchors.leftMargin: scaleX(1)
               color:"white"
               opacity: .75
            }
        }

    Row{
        id:mainrow
        height: childrenRect.height
        width: childrenRect.width
        spacing: scaleX(2)
        anchors.top:headerrow.bottom
        anchors.topMargin: scaleY(2)
        anchors.horizontalCenter: parent.horizontalCenter

        NonEPGPlaylist{ id:playlist}

        Row {
            id:metarow
            width: childrenRect.width
            height: childrenRect.height
            spacing: scaleX(.5)

            Column
            {
                id:metadata
                height: scaleY(30)
                width: childrenRect.width

                Image {
                    id: nowplayingimage

                    width: scaleX(20)
                    height: scaleY(45)
                    fillMode: Image.PreserveAspectFit
                    source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
                }
                Rectangle{
                    id:trackdata
                    height: childrenRect.height
                    width: metadata.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: title.bottom
                    color:style.highlight1
                    clip:true

                    Column{
                        spacing: scaleY(1)


                  /*      Text {
                            id: artist

                            text: qsTr("Artist :") + dcenowplaying.qs_mainTitle
                            font.family: "Droid Sans"
                            wrapMode: "WrapAtWordBoundaryOrAnywhere"
                            font.bold: true
                            smooth: true
                           font.pixelSize: scaleY(2)
                        }


                        Text {
                            id: album

                            wrapMode: "WrapAtWordBoundaryOrAnywhere"
                            text: qsTr("Album: ") + dcenowplaying.qs_mainTitle2
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                          font.pixelSize: scaleY(2)

                        }

                        Text {
                            id: title

                            wrapMode: "WrapAtWordBoundaryOrAnywhere"
                            text: qsTr("Title: ") + dcenowplaying.qs_subTitle
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                           font.pixelSize: scaleY(2)
                        }

                        Text {
                            id: track

                            wrapMode: "WrapAtWordBoundaryOrAnywhere"
                            text: qsTr("Track: ") + dcenowplaying.track
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                           font.pixelSize: scaleY(2)
                        }
                        Text {
                            id: genre

                            wrapMode: "WrapAtWordBoundaryOrAnywhere"
                            text: qsTr("Genre(s): ") + dcenowplaying.genre
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                            font.pixelSize: scaleY(2)
                        }

                        Text {
                            id: synopsis

                            wrapMode: "WrapAtWordBoundaryOrAnywhere"
                            text: qsTr("Review: ") + dcenowplaying.synop
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                            font.pixelSize: scaleY(2)
                        }
                        Text {
                            id: release

                            wrapMode: "WrapAtWordBoundaryOrAnywhere"
                            text: qsTr("Released: ") + dcenowplaying.releasedate
                            font.family: "Droid Sans"
                            font.bold: true
                            smooth: true
                            font.pixelSize: scaleY(2)
                        }
                        */
                    }
                }
            }
            Remote_lighting_controls{ id: remote_lighting_controls1; }
            Remote_Audio_controls{ id: remote1; }
        }
    }
    Row{
        id:controlrow
        anchors.bottom: parent.bottom
        anchors.bottomMargin: scaleY(2)
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
                    buttontext: qsTr("Bookmarks")
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

