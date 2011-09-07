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
        // anchors.centerIn: lightingrow

        HomeButtonDelegate{id:mediaDelegate}
        Flickable{
            id:mediaflick
            height: scaleY(16)
            width: scaleX(100)
            anchors.centerIn: parent
            contentHeight: style.buttonH
            contentWidth: (style.buttonW + 5) * (mediaflick.children.width)
            clip: true
            flickableDirection: "HorizontalFlick"

            Row {
                id: guide

                spacing:5


                ButtonSq {
                    id: rowheader
                    height: scaleY(16)
                    width: scaleX(9)
                    color: style.homescreenfloorplanbuttoncolor
                    radius: style.but_smooth
                    buttontext: ""

                    Image {
                        id: mHeaderImg
                        source: "../../../img/icons/kmix.png"
                        height: scaleY(16)
                        width: scaleX(9)
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        id: mousearea1
                        anchors.fill: parent
                        onClicked:gotoQScreen("Screen_3.qml")
                    }
                }

                ButtonSq {
                    id: now_playing
                    visible: dcenowplaying.b_mediaPlaying ? true : false
                    height: scaleY(16)
                    width: scaleX(9)

                    color: dcenowplaying.b_mediaPlaying ? "green" : "red"
                    radius: style.but_smooth
                    buttontext: dcenowplaying.qs_mainTitle

                    MouseArea{
                        anchors.fill: parent
                        onClicked:gotoQScreen(dcenowplaying.qs_screen)
                    }
                }

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
