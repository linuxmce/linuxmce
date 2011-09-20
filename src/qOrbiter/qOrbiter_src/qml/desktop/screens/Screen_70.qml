import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: storedvideoremote

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
        spacing: 1
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
                        font.pixelSize: scaleY(2)
                        color: style.highlight1

                    }
                }
                Image {
                    id: nowplayingimage
                    width: scaleX(35)
                    height: scaleY(35)
                    fillMode: Image.PreserveAspectFit
                    source: "image://updateobject/"+dcenowplaying.m_iplaylistPosition
                }


                Text {
                    id: programtext
                    width: scaleX(35)
                    text: qsTr("Program :") + dcenowplaying.tvProgram
                    font.family: "Droid Sans"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
                }


                Text {
                    id: episode
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Episode: ") + dcenowplaying.episode
                    font.family: "Droid Sans"
                    font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
                }

                Text {
                    id: genre
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Genre(s): ") + dcenowplaying.genre
                    font.family: "Droid Sans"
                    font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
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
                    font.pixelSize: scaleY(2)
                    elide: "ElideRight"

                    MouseArea{
                        anchors.fill: starring
                        hoverEnabled: true
                        onEntered: { starring.elide = "ElideNone" ; }
                        onExited: {starring.elide = "ElideRight"; }
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
        height: childrenRect.height
        width: childrenRect.width
        anchors.horizontalCenter: storedvideoremote.horizontalCenter
        VideoControls {
            id: videocontrols1
        }
    }

    HomeButton{anchors.right: parent.right; anchors.top:parent.top}


}

