import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs

Rectangle {
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
    color: "transparent"

    Text {
        id: bignowplaying
        text: dcenowplaying.qs_mainTitle
        anchors.centerIn: parent
        font.pixelSize: scaleY(5)
        opacity: .5
        style: Text.Sunken

    }

    Component.onCompleted:setNowPlayingData()

    //main 'now playing rect containing all the other items



    Column{
        anchors.right: storedaudioremote.right
        anchors.rightMargin: scaleX(1)
        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }
    }

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
        width: scaleX(95)
        spacing: scaleX(.5)
        anchors.top:headerrow.bottom
        anchors.topMargin: scaleY(2)
        anchors.horizontalCenter: parent.horizontalCenter

        NonEPGPlaylist{ id:playlist; visible: true}

        Row {
            id:metarow
            width: childrenRect.width
            height: childrenRect.height

            spacing: scaleX(.25)
            Rectangle{
                id:imageholder
                height:childrenRect.height
                width:scaleX(30)
                color: "transparent"
                anchors.top:parent.top

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
                    width: scaleX(30)
                    height:scaley(30)
                    source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition
                    anchors.horizontalCenter: parent.horizontalCenter
                    smooth: true
                }

                Image {
                    id: npmask
                    source: "../images/transparencymask.png"
                    anchors.fill: nowplayingimage
                    opacity: .5

                }


            }

            Rectangle{
                id:textrect
                visible: true
                height: childrenRect.height
                width: scaleX(40)
                color:"transparent"
                anchors.left:  imageholder.left;
                anchors.top:  imageholder.bottom

                clip:true
                Column{
                    spacing: scaleY(.5)
                    width: scaleX(35)
                    height: 25
                    Text {
                        id: starring
                        width: scaleX(40)
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: dcenowplaying.performerlist
                        font.family: "Droid Sans"
                        font.bold: true
                        smooth: true
                        font.pixelSize: scaleY(4)
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
                        id: generaltitle
                        width: scaleX(35)
                        text:  dcenowplaying.mediatitle
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        smooth: true
                        font.pixelSize: scaleY(3.5)
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
                        //font.bold: true
                        font.italic: true
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
                        font.italic: true
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
                        font.italic: true
                        smooth: true
                        font.pixelSize: scaleY(2)
                        visible:  dcenowplaying.releasedate =="" ? false: true

                    }

                }
            }
        }
    }



            VideoControls {
                id: videocontrols1
                anchors.bottom:optionrow.top
            }


            Row{
                id:optionrow
                height: childrenRect.height
                width: style.orbiterw
                spacing: scaleX(5)
                anchors.bottom: storedaudioremote.bottom

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
                    buttontext: qsTr("Resend AV Codes")
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  {
                            MyJs.createAvComponent("../components/Avcodes.qml", storedaudioremote)
                        }
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

