import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"


Rectangle {

    // property alias synText:
    id: satcableboxremote
    height: manager.appHeight
    width: manager.appWidth
    color:"transparent"


    Connections{
        target:dcenowplaying
        onPlayListPositionChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }



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
                color: skinstyle.bgcolor

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

                NowPlayingImage {
                    id: nowplayingimage
                    width: scaleX(35)
                    height: scaleY(35)
                    anchors.top: gradientheader.bottom

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

                    color: skinstyle.bgcolor
                    anchors.top:nowplayingimage.bottom
//                    LiveTvData {
//                    }
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
                TvButtons {
                }
            }
        }

}
