import QtQuick 1.0
import "../components"

Rectangle {
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

    id: storedaudioremote
    height: style.orbiterH
    width: style.orbiterW
    radius: 0
    opacity: 1
    color: "transparent"
    Image {
        id: background
        source: "../../../img/icons/orbiterbg.png"
    }
    Component.onCompleted:setNowPlayingData()

    BorderImage {
        id: remoteborder
        horizontalTileMode: BorderImage.Repeat
        source: "../../../img/icons/drpshadow.png"
        anchors.fill: remotecontainer
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }

    Rectangle{
        id:remotecontainer
        height:headerrow.height + mainrow.height + controlrow.height + scaleY(10)
        width: mainrow.width + scaleX(2)
        clip:true
        radius: 5
        // border.color: style.highlight1
        // border.width: 5
        // color: style.lighthighlight
        anchors.centerIn: parent

        Image {
            id: panelimg
            source: "../../../img/icons/displaypanel.png"
            height: parent.height
            width: parent.width

        }


        //main 'now playing rect containing all the other items

        Row{
            id:headerrow
            height:childrenRect.height
            width: parent.width-1
            anchors.top: parent.top
            anchors.topMargin: scaleY(1)
            Text {
                id: timecode
                height:scaleY(2.15)
                text: qsTr("Speed: ") + dcenowplaying.qs_playbackSpeed +" || " +dcenowplaying.timecode + qsTr(" of ") + dcenowplaying.duration
                font.family: "Droid Sans"
                font.pixelSize: scaleY(2.15)
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold: true
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

            NonEPGPlaylist{ id:playlist; visible: false}

            Row {
                id:metarow
                width: childrenRect.width
                height: childrenRect.height
                spacing: scaleX(.5)

                Column
                {
                    id:metadata
                    height: childrenRect.height
                    width: childrenRect.width

                    Rectangle{
                        id:imageholder
                        height:childrenRect.height
                        width:scaleX(35)
                        color: "transparent"
                        anchors.top:parent.top

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
                            width: dcenowplaying.aspect=="wide"? scaleX(27) : scaleX(15)
                            height:dcenowplaying.aspect=="wide"? scaleY(35) : scaleY(40)
                            source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
                            anchors.horizontalCenter: parent.horizontalCenter
                            smooth: true
                        }
                    }

                    BorderImage {
                        id: texrectborder
                        horizontalTileMode: BorderImage.Repeat
                        source: "../../../img/icons/drpshadow.png"
                        anchors.fill: textrect
                        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                        border { left: 10; top: 10; right: 10; bottom: 10 }
                        smooth: true
                        visible: textrect.visible


                    }

                    Rectangle{
                        id:textrect
                        visible: true
                        height: childrenRect.height
                        width: childrenRect.width
                        color:style.highlight1
                        anchors.left: dcenowplaying.aspect == "wide"? imageholder.left : imageholder.right;
                        anchors.top:  dcenowplaying.aspect == "wide"? imageholder.bottom: imageholder.top
                        anchors.bottomMargin: dcenowplaying.aspect == "wide"? 10 : 20
                        clip:true
                        Column{
                            spacing: scaleY(1.5)
                            width: childrenRect.width
                            height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(65)
                            Text {
                                id: generaltitle
                                width: scaleX(35)
                                text: qsTr("Title: ") + dcenowplaying.mediatitle
                                font.family: "Droid Sans"
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"

                                smooth: true
                                font.pixelSize: scaleY(2)
                                visible:  dcenowplaying.mediatitle =="" ? false: true
                            }

                            Text {
                                id: programtext
                                width: scaleX(35)
                                text: qsTr("Album: ") + dcenowplaying.album
                                font.family: "Droid Sans"
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"

                                smooth: true
                                font.pixelSize: scaleY(2)
                                visible:  dcenowplaying.album =="" ? false: true
                            }

                            Text {
                                id: episode
                                width: scaleX(35)
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                text: qsTr("Track: ") + dcenowplaying.track
                                font.family: "Droid Sans"

                                smooth: true
                                font.pixelSize: scaleY(2)
                                visible:  dcenowplaying.track =="" ? false: true
                            }

                            Text {
                                id: genre
                                width: scaleX(35)
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                text: qsTr("Genre(s): ") + dcenowplaying.genre
                                font.family: "Droid Sans"
                                //font.bold: true
                                smooth: true
                                font.pixelSize: scaleY(2)
                                visible:  dcenowplaying.genre =="" ? false: true
                                MouseArea{
                                    anchors.fill: genre
                                    hoverEnabled: true
                                    onEntered: { genre.elide = "ElideNone" ; }
                                    onExited: {genre.elide = "ElideRight"; }
                                }
                            }
                            Text {
                                id: released
                                width: scaleX(35)
                                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                                text: qsTr("Released: ") + dcenowplaying.releasedate
                                font.family: "Droid Sans"
                                // font.bold: true
                                smooth: true
                                font.pixelSize: scaleY(2)
                                visible:  dcenowplaying.releasedate =="" ? false: true

                            }


                            Text {
                                id: starring
                                width: scaleX(35)
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
                        }
                    }
                }
                Remote_Audio_controls{}
                Remote_lighting_controls{}
            }
        }
        Row{
            id:controlrow
            anchors.top: mainrow.bottom
            anchors.topMargin: scaleY(5)
            height: controlcol.height
            width: childrenRect.width
            anchors.horizontalCenter: parent.horizontalCenter
            Column{
                id:controlcol
                height: childrenRect.height
                width: childrenRect.width
                spacing: scaleY(1)
                VideoControls {
                    id: videocontrols1
                }
                Row{
                    height: childrenRect.height
                    width: childrenRect.width
                    spacing: scaleX(1)

                    AvOptionButton{
                        buttontext: qsTr("Bookmarks")
                    }
                    AvOptionButton{
                        buttontext: qsTr("Manage Playlist")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: playlist.visible = !playlist.visible
                        }
                    }
                    AvOptionButton{
                        buttontext: qsTr("Attributes")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: textrect.visible = !textrect.visible
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
}

