import QtQuick 1.1
import '../components'
import "../../default/lib/handlers"
Rectangle {
    id:stage
   anchors.fill: parent
    color: "transparent"
    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true

        onTriggered: nowPlayingArt.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Component.onCompleted: {
        nonepgplaylistview.currentIndex = dcenowplaying.m_iplaylistPosition
        nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Contain)
    }

    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:{
            nonepgplaylistview.currentIndex = dcenowplaying.m_iplaylistPosition
            nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Contain)
            nowPlayingArt.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        }
    }
    Clock{
        anchors.right: parent.right
        verticalMirror: true
    }

    // Upper left menu
    MenuBar{
        origin: "top-left"
        items: ListModel {
            ListElement {
                text: "Home"
                action: 'setCurrentScreen("Screen_1.qml")'
            }
        }
    }
    Image{
        id: nowPlaying
        x: 0
        y: scaleY(33.33) // 204
        width:  parent.width // 1280
        height: scaleY(87.5) // 630
        //anchors.bottom: parent.bottom
        source:  "../img/home/nowplaying_shadow.png"
        //fillMode:  Image.PreserveAspectFit
        smooth:  true
        visible: dcenowplaying.b_mediaPlaying
        Image{
            x: scaleX(1.4) // 18
            y: scaleY(28.3) // 446 242
            height:  scaleY(36.39) // 262 36.39
            width:  scaleY(36.39) // 262 20.5
            source: "../img/home/nowplaying_back.png"
            //fillMode:  Image.PreserveAspectFit
            smooth:  true
            Image{
                x: scaleY(33.75) // 261 243
                y: scaleY(8.333) // 506 60
                height:  scaleY(24.4) // 176
                width:  scaleY(35.97) // 259
                source: "../img/home/nowplaying_info.png"
                //fillMode:  Image.PreserveAspectFit
                smooth:  true
            }

            Rectangle{
                anchors.centerIn: parent
                width:  parent.width*.90
                height: parent.height*.91
                radius: 6
                //color: "white"
                color: "transparent"
                Image{ // Album art here
                    id: nowPlayingArt
                    anchors.fill: parent
                    source: "image://listprovider/updateobject/"+securityvideo.timestamp
                    fillMode:  Image.PreserveAspectFit
                    smooth:  true
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked:setCurrentScreen(dcenowplaying.qs_screen)
                }
            }
        }

        Image{
            x: scaleX(2.2)
            y: scaleY(29.7)
            height:  scaleY(33.61)
            width:  scaleY(33.61)
            source: "../img/home/nowplaying_overlay.png"
            //fillMode:  Image.PreserveAspectFit
            smooth:  true
        }
        Text{
            id: nowPlayingArtist
            text: dcenowplaying.qs_mainTitle
            x: scaleY(41.11)
            y: scaleY(32.78) // 478 236
            font.family: aeonNowPlaying.name
            //font.capitalization: Font.AllUppercase
            smooth: true
            font.pixelSize: scaleY(2.92) // 21
            color: "white"
        }
        Text{
            id: nowPlayingAlbum
            text: dcenowplaying.qs_mainTitle2
            x: scaleY(41.11)
            y: scaleY(37.22) // 510 268
            font.family: aeonNowPlaying.name
            //font.capitalization: Font.AllUppercase
            smooth: true
            font.pixelSize: scaleY(2.22) // 16
            opacity:  .7
            color: "white"
        }
        Text{
            id: nowPlayingStatus
            text: "NOW PLAYING"
            x: scaleY(41.11)
            y: scaleY(54.44) // 634 392
            font.family: aeonNowPlaying.name
            smooth: true
            font.pixelSize: scaleY(2.22) // 16
            opacity:  .7
            color:  "#749a9a"
        }
        Text{
            id: nowPlayingTrack
            text: dcenowplaying.qs_subTitle
            x: scaleY(41.11)
            y: scaleY(56.67) // 650 408
            font.family: aeonNowPlaying.name
            //font.capitalization: Font.AllUppercase
            smooth: true
            font.pixelSize: scaleY(3.06) // 22
            opacity:  .7
            color: "white"
        }
    }

    Image {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: scaleX(1.95) // 25
        width: scaleX(31.25) // 400
        height: scaleY(83.33) // 600
        source: '../img/list/listpanel_back.png'
        smooth: true
        Rectangle {
            color: "transparent"
            width: parent.width*.95
            anchors.horizontalCenter: parent.horizontalCenter
            height: parent.height*.88
            anchors.top: parent.top
            anchors.topMargin: scaleY(2.78) // 20
            clip: true

            ListView{
                id:nonepgplaylistview
                anchors.fill: parent

                highlightFollowsCurrentItem: true
                highlight: Rectangle { color: "lightgray"; radius: scaleY(1.39) }
                clip: true
                interactive: true
                flickableDirection: "VerticalFlick"
                model: manager.getDataGridModel("Playist", 18)
                delegate:
                    Rectangle {
                    width:nonepgplaylistview.width
                    height: nonepgplaylistview.height/10
                    color: "transparent"

                    Text {
                        id: position
                        text: name
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.rightMargin: 5
                        opacity: .5
                        color: "black"
                        anchors.right: parent.right
                        font.family: aeonNowPlaying.name
                        //font.capitalization: Font.AllUppercase
                        smooth: true
                        font.pixelSize: parent.height*.5
                    }

                    Text {
                        text: dceindex
                        color: "black"
                        //width: parent.width
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: 5
                        font.family: aeonNowPlaying.name
                        //font.capitalization: Font.AllUppercase
                        smooth: true
                        font.pixelSize: parent.height*.5
                    }

                    PlaylistClickedHandler{

                    }
                }
            }
        }
    }

    Rectangle {
        y: scaleY(40)
        width: childrenRect.width
        height: childrenRect.height
        color: "transparent"

        Row{
            spacing: 5


            Rectangle {
                id:back
                width: style.stdbuttonh
                height: style.stdbuttonw
                radius: 25
                Text {
                    id: media_button_text
                    x: 0
                    y: 26
                    text: "back"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: backicon
                    source: "../../../../img/icons/player_end.png"
                    rotation: 180
                    height: parent.height
                    width: parent.width
                }
             ChangeTrackHandler{
                 trackDirection: "-1"
             }
            }

            Rectangle {id:rw ;

                width: style.stdbuttonh
                height: style.stdbuttonw
                radius: 25
                Text {
                    x: 0
                    y: 26
                    text: "RW"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: rwicon
                    source: "../../../../img/icons/player_rew.png"
                    height: parent.height
                    width: parent.width
                }
               PlaybackSpeedHandler{
                   speed: -2
               }
            }

            Rectangle {
                id:play ;

                width: style.stdbuttonh
                height: style.stdbuttonw
                radius: 25
                Text {
                    x: 0
                    y: 26
                    text: "Play"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: playicon
                    source: "../../../../img/icons/player_play.png"
                    height: parent.height
                    width: parent.width

                }
                PauseButtonHandler{

                }


            }

            Rectangle {id:ff ;

                width: style.stdbuttonh
                height: style.stdbuttonw
                radius: 25
                Text {
                    x: 0
                    y: 26
                    text: "FF"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: fficon
                    source: "../../../../img/icons/player_fwd.png"
                    height: parent.height
                    width: parent.width
                }
                PlaybackSpeedHandler{
                    speed: 2
                }

            }
            Rectangle {id:next ;

                width: style.stdbuttonh
                height: style.stdbuttonw
                radius: 25
                Text {
                    x: 0
                    y: 26
                    text: "Next"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: nexticon
                    source: "../../../../img/icons/player_end.png"
                    height: parent.height
                    width: parent.width
                }
                ChangeTrackHandler{
                    trackDirection: "+1"
                }
            }


            Rectangle {
                id: stop

                color: "#c28585"
                width: style.stdbuttonh
                height: style.stdbuttonw
                radius: 25
                Text {
                    x: 0
                    y: 26
                    text: "Stop"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: stopicon
                    source: "../../../../img/icons/player_stop.png"
                    height: parent.height
                    width: parent.width
                }
               StopMediaHandler{

               }
            }
        }
    }
}
