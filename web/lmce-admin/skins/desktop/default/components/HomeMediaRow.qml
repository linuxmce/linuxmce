import QtQuick 1.0

Item {
    height: scaleY(17)
    width: scaleX(100)
    clip:true

    Rectangle{
        height: scaleY(17)
        width: scaleX(100)
        clip:true
        color:"transparent"
        radius: 20


        HomeButtonDelegate{id:mediaDelegate}

        Row {
            id: guide
            spacing:5
            x: scaleX(2)

            ButtonSq {
                id: rowheader
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                height: scaleY(13)
                width: scaleX(8)
                color: style.homescreenfloorplanbuttoncolor
                radius: style.but_smooth
                buttontext: ""
                Image {
                    id: buttonbg
                    source: "../../../img/icons/buttonoverlay.png"
                    height: parent.height
                    width: parent.width
                }
                Image {
                    id: mHeaderImg
                    source: "../../../img/icons/kmix.png"
                    height: parent.height
                    width: parent.width
                }

                MouseArea{
                    id: mousearea1
                    anchors.fill: parent
                    onClicked:showfloorplan(3)
                }
            }

            ButtonSq {
                id: now_playing
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                visible: dcenowplaying.b_mediaPlaying ? true : false
                height: scaleY(13)
                width: scaleX(8)

                radius: style.but_smooth
                buttontext: dcenowplaying.qs_mainTitle +" \n " + dcenowplaying.timecode

                MouseArea{
                    anchors.fill: parent
                    onClicked:screenchange(dcenowplaying.qs_screen)
                }
            }
        Flickable{
            id:mediaflick
            height: scaleY(16)
            width: scaleX(79)
            contentHeight: style.buttonH
            contentWidth: (style.buttonW + 5) * (mediaflick.children.width)
            clip: true
            flickableDirection: "HorizontalFlick"

                ListView{
                    id: mediaScenarios
                    height: scaleY(style.buttonH)
                    width: stage.width

                    model: currentRoomMedia
                    orientation:ListView.Horizontal
                    spacing: 5
                    delegate: mediaDelegate
                    interactive: false

                }
            }
        }
    }
}
