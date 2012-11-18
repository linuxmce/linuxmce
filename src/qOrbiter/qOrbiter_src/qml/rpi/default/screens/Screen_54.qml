import QtQuick 2.0
//import Qt.labs.shaders 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle {
    id: storedaudioremote

    StyledText {
        id: messages
        text: manager.mediaResponse
        font.pixelSize: scaleY(4)
        anchors.bottom: parent.bottom
    }

    Connections{
        target:dcenowplaying
        onImageChanged:nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }
 Component.onCompleted: manager.setBoundStatus(true)

    height: manager.appHeight
    width: manager.appWidth 
    color: "transparent"

    Rectangle{
        id:filler
        color: "lightgrey"
        anchors.fill:parent
        opacity: .15

    }

    StyledText {
        id: bignowplaying
        text: dcenowplaying.qs_mainTitle
        anchors.centerIn: parent
        font.pixelSize: scaleY(5)
        font.family: keyFont.name
        opacity: .5
       font.bold: true

    }

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

        StyledText {
            id: timecode
            height:scaleY(2.15)
            text: qsTr("Speed: ") + dceTimecode.playbackSpeed +" || " +dceTimecode.qsCurrentTime + qsTr(" of ") + dceTimecode.qsTotalTime
            font.family: keyFont.name
            font.pixelSize: scaleY(2.15)

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
                    width: dcenowplaying.aspect=="wide"? scaleX(18) : scaleX(18)
                    height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(30)
                    source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition

                    smooth: true
                    Image {
                        id: npmask
                        source: "../img/icons/transparencymask.png"
                        anchors.fill: nowplayingimage
                        opacity: .5
                    }
                }


            }

            Rectangle{
                id:textrect
                visible: true
                height: childrenRect.height
                width: scaleX(40)
                color:"transparent"
//                anchors.left:  imageholder.right;
//                anchors.top:  imageholder.top
//                anchors.bottomMargin: dcenowplaying.aspect == "wide"? 10 : 20
                clip:true
                Column{
                    spacing: scaleY(.5)
                    width: childrenRect.width
                    height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
                    StyledText {
                        id: starring
                        width: scaleX(40)
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        text: dcenowplaying.performerlist
                        font.family:keyFont.name
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
                    StyledText {
                        id: generaltitle
                        width: scaleX(35)
                        text:  dcenowplaying.mediatitle
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        smooth: true
                        font.pixelSize: scaleY(3.5)
                        visible:  dcenowplaying.mediatitle =="" ? false: true
                    }

                    StyledText {
                        id: programtext
                        width: scaleX(35)
                        text: qsTr("Album: ") + dcenowplaying.album
                        font.family: "Droid Sans"
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        smooth: true
                        font.pixelSize: scaleY(2)
                        visible:  dcenowplaying.album =="" ? false: true
                    }

                    StyledText {
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

                    StyledText {
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
                    StyledText {
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


    MediaScrollBar{id:media_transit; anchors.bottom: controlrow.top; anchors.horizontalCenter: controlrow.horizontalCenter; anchors.bottomMargin: scaleY(2)}
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
                        onClicked: playlist.optionVisible ? playlist.optionVisible =false: playlist.optionVisible=true
                    }
                }
                AvOptionButton{
                    buttontext: qsTr("Resend AV Codes")
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  {
                            MyJs.createStageComponent("../components/Avcodes.qml", storedaudioremote)
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
    }

}

