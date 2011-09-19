import QtQuick 1.0

Item {
    height: scaleY(17)
    width: scaleX(100)
    clip:true

    Rectangle{
        height: scaleY(17)
        width: scaleX(100)
        clip:true
        color:"midnightblue"
        radius: 20
        Image {
            id: rowbgimg

            source: "../../../img/icons/rowbg.png"
            width: parent.width
            height: parent.height

        }
        // anchors.centerIn: lightingrow

        HomeButtonDelegate{id:mediaDelegate}

        Row {
            id: guide
            spacing:5
            anchors.left: parent.left
            anchors.leftMargin: scaleX(2)

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
                    anchors.fill: rowheader
                    height: parent.height
                    width: parent.width
                }
                Image {
                    id: mHeaderImg
                    source: "../../../img/icons/kmix.png"
                    height: parent.height
                    width: parent.width
                    anchors.centerIn: parent
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

                color: dcenowplaying.b_mediaPlaying ? "green" : "red"
                radius: style.but_smooth
                buttontext: dcenowplaying.qs_mainTitle

                MouseArea{
                    anchors.fill: parent
                    onClicked:screenchange(dcenowplaying.qs_screen)
                }
            }
        Flickable{
            id:mediaflick
            height: scaleY(16)
            width: scaleX(79)
            anchors.left: now_playing.right
            anchors.leftMargin: 4
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
