import QtQuick 1.0

Rectangle {
    width: np_box.width
    height: np_box.height
    id: now_playing

    visible: dcenowplaying.b_mediaPlaying ? true : false
    color:"transparent"

    Image {
        id: np_box
        source: "../img/nowplayingbox.png"
    }
    Image {
        id: nowplayingimage
        height: style.orbiterh
        fillMode: Image.PreserveAspectCrop
        width: style.orbiterw
        anchors.fill: np_box
        source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition

    }
    Text {
        id: np_label
        text: qsTr("NowPlaying")
        font.pixelSize: 12
        anchors.top: np_box.top
        anchors.topMargin: 15
        anchors.left: np_box.left
        anchors.leftMargin: 20
        color: "Green"
    }

    Text{
        text: dcenowplaying.qs_mainTitle
        font.pixelSize: 12
        anchors.top: np_label.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
        width: np_box.width - 40
        anchors.horizontalCenter: np_box.horizontalCenter
    }

    MouseArea{
        anchors.fill: parent
        onClicked:screenchange(dcenowplaying.qs_screen)
    }

    Text {
        id: np
        text:dcenowplaying.timecode
        font.pixelSize: 12
        anchors.bottom: np_label.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.horizontalCenter: np_box.horizontalCenter
        color: "white"

        Rectangle{
                       id:textrect
                       x: -20
                       y: 20
                       visible: true
                       height: childrenRect.height
                       width: np_box.width
                       color:"transparent"
                       anchors.left:  np_box.left
                       anchors.top:  np_box.bottom
                       anchors.bottomMargin: dcenowplaying.aspect == "wide"? 10 : 20
                       clip:true


                       Column{
                           id: column1
                           x: -27
                           y: 12
                           spacing: scaleY(.5)
                           width: childrenRect.width
                           height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
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
color: "Green"
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
                               color: "Green"
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
                               color: "Green"
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
                               color: "Green"
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
                               color: "Green"
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
                               color: "Green"
                               font.pixelSize: scaleY(2)
                               visible:  dcenowplaying.releasedate =="" ? false: true

                           }

                       }
                   }

    }
    states: [
        State {
            name: "livetv"

            PropertyChanges {
                target: column1
                x: 12
                y: 0
            }

            PropertyChanges {
                target: textrect
                x: -38
                y: 27
            }
        },
        State {
            name: "storedaudio"

            PropertyChanges {
                target: column1
                x: -39
                y: 29
            }

            PropertyChanges {
                target: textrect
                x: -27
                y: 18
            }
        },
        State {
            name: "storedvideo"
        },
        State {
            name: "game"
        },
        State {
            name: "dvd"
        }
    ]


}
