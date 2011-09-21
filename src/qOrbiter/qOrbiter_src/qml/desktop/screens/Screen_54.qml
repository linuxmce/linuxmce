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
    Component.onCompleted:setNowPlayingData()


    //main 'now playing rect containing all the other items

    Row{
        id:mainrow
        height: childrenRect.height
        width: childrenRect.width
        spacing: scaleX(2)
        anchors.top:parent.top
        anchors.topMargin: scaleY(5)
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
                height: scaleY(75)
                width: childrenRect.width

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
                    height:scaleY(5)
                    text:qsTr("Speed: ") + dcenowplaying.qs_playbackSpeed
                    font.family: "Droid Sans"
                    font.pixelSize: 12

                }
            }
                Image {
                    id: nowplayingimage
                    width: scaleX(45)
                    height: scaleY(45)
                    fillMode: Image.PreserveAspectFit
                    source: "image://updateobject/"+dcenowplaying.m_iplaylistPosition
                }

                    Text {
                        id: artist
                        height: scaleY(5)
                        text: qsTr("Artist :") + dcenowplaying.qs_mainTitle
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        font.bold: true
                        smooth: true
                        font.pixelSize: 16
                    }


                    Text {
                        id: album
                         height: scaleY(5)
                         wrapMode: "WrapAtWordBoundaryOrAnywhere"
                         text: qsTr("Album: ") + dcenowplaying.qs_mainTitle2
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true                      
                        font.pixelSize: 16

                    }

                    Text {
                        id: title
                         height: scaleY(5)
                         wrapMode: "WrapAtWordBoundaryOrAnywhere"
                         text: qsTr("Title: ") + dcenowplaying.qs_subTitle
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true                      
                        font.pixelSize: 16
                    }

                    Rectangle{
                        id:trackdata
                        height: scaleY(15)
                        width: metadata.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: title.bottom
                        color:style.lighthighlight
                        opacity: .25
                        clip:true
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
                        buttontext: qsTr("Zoom & Aspect")
                    }
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

