import QtQuick 1.1


Item{
    height: scaleY(15)
    width: scaleX(100)
    clip:false

    Row {
        id: guide
        spacing:5
        x: scaleX(2)

        Rectangle {
            id: rowheader
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            height: scaleY(13)
            width: scaleX(8)
            color: "transparent"
            radius: skinStyle.but_smooth

            Image {
                id: mHeaderImg
                source: "../img/ui3/mediabig.png"
                height: parent.height
                width: parent.width
            }

            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked:{
                    manager.setFloorplanType(3)
                    manager.showfloorplan(3)

                }
            }
        }

        ButtonSq {
            id: now_playing
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            visible: dcenowplaying.b_mediaPlaying ? true : false
            height: scaleY(13)
            width: scaleX(8)

            radius: skinStyle.but_smooth
            buttontext: dcenowplaying.qs_mainTitle +" \n " + dceTimecode.qsCurrentTime

            MouseArea{
                anchors.fill: parent
                onClicked:manager.setCurrentScreen(dcenowplaying.qs_screen)
            }
        }
        Flickable{
            id:mediaflick
            height: scaleY(16)
            width: scaleX(79)
            contentHeight: skinStyle.buttonH
            contentWidth: (skinStyle.buttonW + 5) * (mediaflick.children.width)
            clip: true
            flickableDirection: "HorizontalFlick"
            anchors.verticalCenter: parent.verticalCenter

            ListView{
                id: mediaScenarios
                height: scaleY(skinStyle.buttonH)
                width: stage.width

                model: currentRoomMedia
                orientation:ListView.Horizontal
                spacing:5
                delegate:   HomeButtonDelegate{id:mediaDelegate}
                interactive: false

            }
        }
    }
}

