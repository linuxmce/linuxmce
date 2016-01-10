import QtQuick 1.1
import org.linuxmce.enums 1.0
import "../components"
Item{

    state:"fallback"

    NowPlayingRemoteImage {
        id: nowplayingimage
    }

    Column{
        id:basicInfo
        anchors.margins: 8
        StyledText {
            id: basicTitle
            width: parent.width
            text:  dcenowplaying.qs_mainTitle
            elide: Text.ElideRight
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.pixelSize: scaleY(5)
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true
            color: "white"
        }
        StyledText {
            id: musicPerformer
            width: parent.width
            text: qsTr("Artist: ")+ dcenowplaying.performerlist
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            elide: Text.ElideRight
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.performerlist =="" ? false: true
            color: "white"
        }
        StyledText {
            id: episode
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Episode: ") + dcenowplaying.episode
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.episode =="" ? false: true
            color:"silver"

        }
        StyledText {
            id: track
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Track#: ") + dcenowplaying.track
            //font.italic: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.track =="" ? false: true
        }

        StyledText {
            id: album
            width: parent.width
            text: qsTr("Album: ")+ dcenowplaying.album + " | " + dcenowplaying.releasedate
            elide: Text.ElideRight
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.album =="" ? false: true
            color: "white"
        }


        StyledText {
            id: released
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: "Year:" +dcenowplaying.releasedate
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.releasedate =="" ? false: true
            color:"silver"

        }
    }

//    Item{
//        anchors{
//            left:parent.left
//            right:parent.right
//            top:nowplayingimage.bottom
//            bottom:parent.bottom
//        }

//        StyledText {
//            id: performer
//            width: parent.width / 2
//            text: qsTr("Starring: \n")+ dcenowplaying.performerlist
//            wrapMode: "WrapAtWordBoundaryOrAnywhere"
//            elide: Text.ElideRight
//            smooth: true

//            visible:  dcenowplaying.performerlist =="" ? false: true
//            color: "white"
//        }

//        StyledText {
//            id: genre
//            width: parent.width /2
//            anchors.right: parent.right
//            wrapMode: "WrapAtWordBoundaryOrAnywhere"
//            text: qsTr("Genre(s):\n ") + dcenowplaying.genre
//            // elide: Text.ElideRight
//            smooth: true

//            visible:  dcenowplaying.genre =="" ? false: true
//            color:"white"
//            MouseArea{
//                anchors.fill: genre
//                hoverEnabled: true
//                onEntered: { genre.elide = "ElideNone" ; }
//                onExited: {genre.elide = "ElideRight"; }
//            }
//        }


//    }

//    Column{
//        id:textcol
//        spacing: scaleY(1.5)

//        visible:false

//        Text {
//            id: generaltitle
//            width: parent.width
//            text:  dcenowplaying.qs_mainTitle
//            elide: Text.ElideRight
//            wrapMode: "WrapAtWordBoundaryOrAnywhere"
//            smooth: true
//            font.pixelSize: scaleY(5)
//            visible:  dcenowplaying.qs_mainTitle =="" ? false: true
//            color: "white"
//        }

//        Text {
//            id: tracktitle
//            width: parent.width
//            elide: Text.ElideRight
//            text: qsTr("Title: ") + dcenowplaying.mediatitle
//            wrapMode: "WrapAtWordBoundaryOrAnywhere"
//            font.pixelSize: scaleY(4)
//            visible:  dcenowplaying.mediatitle =="" ? false: true
//            color: "white"
//        }

//        Text {
//            id: synopsis
//            width: parent.width
//            wrapMode: "WrapAtWordBoundaryOrAnywhere"
//            text: qsTr("Synopsis: ") + dcenowplaying.synop
//            font.family: "Droid Sans"
//            font.bold: true
//            smooth: true
//            font.pixelSize: scaleY(3)
//            // elide: "ElideRight"
//            visible:  dcenowplaying.synop =="" ? false: true
//            color:"silver"
//        }
//    }

    states: [
        /* default state */
        State {
            name: "fallback"


            AnchorChanges{
                target:nowplayingimage
                anchors{
                    top:parent.top
                    left:parent.left
                }
            }

            AnchorChanges{
                target:basicInfo
                anchors{
                    top:nowplayingimage.bottom
                    left:nowplayingimage.left
                    right:parent.right
                    bottom:parent.bottom
                }
            }

//            AnchorChanges{
//                target: textcol
//                anchors{
//                    top:nowplayingimage.bottom
//                    left:parent.left
//                    right:parent.right
//                    bottom:parent.bottom
//                }
//            }
        },
        State {
            name: "storedaudio"
            when:manager.i_current_mediaType===Mediatypes.STORED_AUDIO
            extend:"fallback"

            PropertyChanges{
                target:basicTitle
                visible:false
            }
        },
        State {
            name: "storedvideo"
            when:manager.i_current_mediaType===Mediatypes.STORED_VIDEO
            extend:"fallback"
            PropertyChanges {
                target: musicPerformer
                visible:false

            }
            PropertyChanges{
                target:generaltitle
                visible:false
            }
            PropertyChanges{
                target:tracktitle
                visible:false
            }
        },
        State {
            name: "storedvideowide"
            extend:"storedvideo"
            when:manager.i_current_mediaType===Mediatypes.STORED_VIDEO && dcenowplaying.aspect==="WIDE"

        },
        State {
            name: "storedvideoposter"
            extend:"storedvideo"
            when:manager.i_current_mediaType===Mediatypes.STORED_VIDEO && dcenowplaying.aspect==="POSTER"
        }
    ]

}
