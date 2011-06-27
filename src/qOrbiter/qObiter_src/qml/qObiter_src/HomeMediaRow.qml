import QtQuick 1.0

Rectangle {
    signal showDatagrid( string msg)
    signal gotoScreen( string screen)    
    property alias nowplayingtext:now_playing.buttontext
    property alias rowlabel: rowheader.buttontext

    height: 80
    width: parent.width
color:style.rowbgColor

Flickable{
    id:mediaflick
    interactive: true
    height: 80
    width: parent.width
    contentHeight: 80
    contentWidth: childrenRect * 1
    clip: true

    Row {
        id: guide
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        spacing:5


        ButtonSq {
            id: rowheader
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth

            Image {
                id: mHeaderImg
                source: "../../img/icons/kmix.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent
                opacity: .5
                }

            buttontext: "Media"
            buttontextbold: true
            buttontextfontsize: 12
            buttontextcolor: "white"

            }

        ButtonSq {
            id: now_playing
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""

            }
        ButtonSq {
            id: mythTV
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            Image {
                id: mythIcon
                source: "../../img/icons/Mythtv.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }
            buttontext:""


            }

        ButtonSq {
            id: videos
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth

            Image {
                id: videosIcon
                source: "../../img/icons/xine.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }

            buttontext: "Videos"
            buttontextbold: true
            buttontextfontsize: 12
            buttontextcolor: "white"
            MouseArea{
                anchors.fill: parent
                onClicked: dceObject.requestDataGrid("5||||1,2|0|13|0 | 2 |")
            }


            }
        ButtonSq {
            id: audio
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth

            buttontext: "Audio"
            buttontextbold: true
            buttontextfontsize: 12
            buttontextcolor: "white"

            Image {
                id: audioimg
                source: "../../img/icons/agt_mp3.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }



            MouseArea {
                anchors.fill: parent
                onClicked: dceObject.requestDataGrid("4||||1,2|0|13|0 | 2 |")
            }

            }
        ButtonSq {
            id: games
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth

            Image {
                id: gameimg
                source: "../../img/icons/package_games.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }

            buttontext: "LinuxMCE Games"
            buttontextbold: true
            buttontextfontsize: 10
            buttontextcolor: "black"

            }

        ButtonSq {
            id: hulu
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Hulu Dekstop"

            }

        ButtonSq {
            id: playlists
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Playlists"

            }
        ButtonSq {
            id: drives
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Manage Drives"
            }
        ButtonSq {
            id: tv
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "DirecTv"

            }
        ButtonSq {
            id: ps3
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "PlayStation 3"
            }
         }
    }
}
